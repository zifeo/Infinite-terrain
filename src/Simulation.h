#pragma once

#include "icg_helper.h"

#include "Bezier.h"
#include "Config.h"
#include "Framebuffer.h"
#include "grid/grid.h"
#include "perlinTex/perlinTex.h"
#include "sky/sky.h"
#include "tree/tree.h"
#include "water/water.h"

#include <glm/gtx/transform.hpp>

#include <map>
#include <queue>
#include <stdint.h>

using namespace std;
using namespace glm;

class Simulation {

  private:
    // windows parameters
    int window_width = WINDOW_WIDTH;
    int window_height = WINDOW_HEIGHT;
    double cursor_x = 0;
    double cursor_y = 0;

    // camera displacement
    enum { UP = 0, DOWN, RIGHT, LEFT, LEFT_UP, RIGHT_UP };
    bool arrows_down[6] = {false, false, false, false, false, false};
    float cam_phi = 2.0f;
    float cam_theta = 0.0f;
    vec3 cam_pos = vec3(0, 1, 0);
    vec3 cam_speed = vec3(0, 0, 0);
    float cam_speed_phi = 0.0f;
    float cam_speed_theta = 0.0f;

    bool is_jumping = false;
    float y_speed = 0.0;

    // camera mode
    enum CameraMode { DEFAULT_CAMERA = 0, GROUND, FLIGHT, RECORD, B_PATH };
    CameraMode camera_mode = DEFAULT_CAMERA;

    // MVP
    mat4 projection_matrix;
    mat4 model_matrix;
    mat4 view_matrix;

    // view mode
    enum Mode { DEFAULT = 0, PERLIN, TERRAIN };
    Mode mode = DEFAULT;

    // fps
    int nb_frames = 0;
    float last_frame_time = 0;
    double last_frame_cnt_time = 0;

    // Perlin parameters
    PerlinTex perlin_tex;
    int octave = 9;
    float lacunarity = 2;
    float fractal_increment = 1.25;

    // trees
    typedef struct {
        vec3 pos;
        TreeType type;
    } TreeStruct;

    // chunks
    typedef struct {
        FrameBuffer tex;
        GLuint perlinBuffer_tex_id;
        int x;
        int y;
        std::vector<TreeStruct> treeList;
        bool tmpFlag;
    } ChunkTex;

    map<uint64_t, ChunkTex> chunk_map;

    // objects
    Grid grid;
    Water water;
    Tree tree;
    Sky sky;

    Bezier path;
    Bezier cam;
    Bezier speed;

    bool start_path = true;
    double bezier_time;

    bool start_record = true;
    bool recording = false;

    vec3* cam_pos_first;
    vec3* cam_rot_first;

    double next_speed = 1;
    double speed_changed = -1;
    double curr_speed = 1;

    // water reflection
    FrameBuffer water_reflection;

    vec2 biome_position[BIOME_COUNT] = {vec2(0.5, 0.5), vec2(0.75, 0.35), vec2(0.35, 0.65), vec2(0.3, 0.2),
                                        vec2(0.5, 0.2)}; // if changes, need to copy to shaders !

    vector<TreeType> biome_trees[BIOME_COUNT];
    float biome_tree_count[BIOME_COUNT] = {1.f, 0.3f, 0.4f, 0.5f};

  public:
    /* ********** States ********** */

    void init(GLFWwindow *window) {

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        onResize(window);

        perlin_tex.Init();
        grid.Init();
        GLuint water_reflection_tex_id = water_reflection.Init(window_width, window_height, false, GL_RGB8, GL_RGB);
        water.Init(water_reflection_tex_id);
        sky.Init();
        tree.Init();

        biome_trees[NORMAL_BIOME].push_back(NORMAL_TREE);
        biome_trees[DESERT_TREE].push_back(DESERT_TREE);
        biome_trees[DESERT_TREE].push_back(CACTUS);
        biome_trees[HIGH_MOUNTAIN].push_back(NORMAL_TREE);
        biome_trees[SEA].push_back(NORMAL_TREE);
    }

    void display() {

        float curr_time = (float)glfwGetTime();
        float frame_time = curr_time - last_frame_time;
        float coef = frame_time / (1 / 60.0f);

        // Measure speed
        ++nb_frames;
        if (curr_time - last_frame_cnt_time >= 1.0) { // over 1 second
            cout << nb_frames << " frames" << endl;
            nb_frames = 0;
            last_frame_cnt_time = curr_time;
        }
        last_frame_time = curr_time;

        // Camera movements
        switch (camera_mode) {

        case DEFAULT_CAMERA:
            cameraMovements(cam_phi, coef);
            break;
        case RECORD:

            if (start_record) {
                path.purge();
                cam.purge();
                vec3 *newpos = new vec3(cam_pos);
                path.addPoint(*newpos);
                vec3 *new_orien = new vec3(cam_phi, cam_theta, 0);
                cam.addPoint(*new_orien);
                cam_pos_first = newpos;
                cam_rot_first = new_orien;
                start_record = false;
                recording = true;
            }

        case FLIGHT:
            // Camera movements
            cameraMovements(cam_phi, coef);
            break;

        case B_PATH:
            if (start_path) {
                // path.print_list();
                // cam.print_list();
                bezier_time = 0;
                start_path = false;
            } else {

                if (speed_changed >= 0) {
                    if (curr_time - speed_changed < 2) {
                        double acc_time =  curr_time - speed_changed;
                        curr_speed = speed.bezierPoint(acc_time).x;
                    } else {
                        speed_changed = -1;
                        curr_speed = next_speed;
                    }
                }

                bezier_time += frame_time*curr_speed;

                if (bezier_time > path.get_nbr_elem() - 1) {
                    start_path = true;

                } else {

                    cam_pos = path.bezierPoint(bezier_time);

                    vec3 newAngles = cam.bezierPoint(bezier_time);
                    cam_phi = newAngles.x;
                    cam_theta = newAngles.y;

                    int chunkCamX = (int)floor((cam_pos.x + 1) / 2);
                    int chunkCamY = (int)floor((cam_pos.z + 1) / 2);

                    float posInChunkX = ((cam_pos.x + 1) / 2) - chunkCamX;
                    float posInChunkY = ((cam_pos.z + 1) / 2) - chunkCamY;

                    map<uint64_t, ChunkTex>::iterator it = chunk_map.find(getKey(chunkCamX, chunkCamY));

                    if (it != chunk_map.end()) {
                        // Sometimes, just before the chunk's generation, there is no ground at the bottom of the camera
                        it->second.tex.Bind();

                        GLfloat r[1];
                        glReadPixels((int)(posInChunkX * TEX_WIDTH), (int)(TEX_HEIGHT - posInChunkY * TEX_HEIGHT), 1, 1,
                                     GL_RED, GL_FLOAT, r);
                        it->second.tex.Unbind();
                        float newHeight = r[0] * 2 - 1 + 0.17f;

                        if (newHeight > cam_pos.y) {
                            cam_pos.y = newHeight;
                        }
                    }
                }
            }
            break;

        case GROUND:
            if (is_jumping) {
                y_speed -= (float)G;
            }

            float old_cam_posY = cam_pos.y;

            cameraMovements((float)M_PI / 2, coef);

            int chunkCamX = (int)floor((cam_pos.x + 1) / 2);
            int chunkCamY = (int)floor((cam_pos.z + 1) / 2);

            float posInChunkX = ((cam_pos.x + 1) / 2) - chunkCamX;
            float posInChunkY = ((cam_pos.z + 1) / 2) - chunkCamY;

            map<uint64_t, ChunkTex>::iterator it = chunk_map.find(getKey(chunkCamX, chunkCamY));

            if (it != chunk_map.end()) {
                // Sometimes, just before the chunk's generation, there is no ground at the bottom of the camera
                it->second.tex.Bind();

                GLfloat r[1];
                glReadPixels((int)(posInChunkX * TEX_WIDTH), (int)(TEX_HEIGHT - posInChunkY * TEX_HEIGHT), 1, 1, GL_RED,
                             GL_FLOAT, r);
                it->second.tex.Unbind();

                float newHeight = r[0] * 2 - 1 + 0.17f;

                if (is_jumping) {
                    cam_pos.y = old_cam_posY + y_speed;

                    if (cam_pos.y < newHeight) {
                        is_jumping = false;
                    }
                } else {
                    cam_pos.y = newHeight;
                }
            }
            break;
        }

        // Display
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float water_height_sh = WATER_HEIGHT + WATER_AMPL * sin(curr_time);
        float water_height = (water_height_sh + 1) / 2;

        view_matrix = lookAt(cam_pos, cam_pos + vecFromRot(cam_phi, cam_theta), vec3(0.0f, 1.0f, 0.0f));
        vec3 cam_pos2 = vec3(cam_pos.x, -cam_pos.y + 2 * water_height_sh, cam_pos.z);
        mat4 view_matrix_reflection =
            lookAt(cam_pos2, cam_pos2 + vecFromRot((float)M_PI - cam_phi, cam_theta), vec3(0.0f, -1.0f, 0.0f));

        switch (mode) {

        case DEFAULT:
            break;
        case PERLIN:
            perlin_tex.Draw(octave, lacunarity, fractal_increment, 0, 0);
            break;
        case TERRAIN:

            water_reflection.Bind();
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_CLIP_DISTANCE0);

                drawChunks(model_matrix, view_matrix_reflection, projection_matrix, curr_time, water_height);
                sky.Draw(translate(projection_matrix * model_matrix * view_matrix_reflection, cam_pos2));
                drawTrees(model_matrix, view_matrix_reflection, projection_matrix, curr_time, water_height_sh);

                glDisable(GL_CLIP_DISTANCE0);
            }
            water_reflection.Unbind();

            drawChunks(model_matrix, view_matrix, projection_matrix, curr_time);
            drawTrees(model_matrix, view_matrix, projection_matrix, curr_time);

            mat4 model = scale(model_matrix, vec3(WATER_SIZE, 1, WATER_SIZE));
            model = translate(model, vec3(cam_pos.x / WATER_SIZE, WATER_HEIGHT, cam_pos.z / WATER_SIZE));
            water.Draw(curr_time, model, view_matrix, projection_matrix, cam_pos);

            sky.Draw(translate(projection_matrix * model_matrix * view_matrix, cam_pos));
            break;
        }

        // cleanup
        // + 1 is because we are in the middle of a chunk
        // / 2 because a chunk is of length 2
        int chunkX = (int)floor((cam_pos.x + 1) / 2);
        int chunkY = (int)floor((cam_pos.z + 1) / 2);

        for (auto &chunk : chunk_map) {
            chunk.second.tmpFlag = false;
        }

        queue<uint64_t> toBeUpdated;
        for (auto &chunk : chunk_map) {
            if ((chunk.second.x / 2 < chunkX - VIEW_DIST || chunk.second.x / 2 > chunkX + VIEW_DIST) ||
                (chunk.second.y / 2 < chunkY - VIEW_DIST || chunk.second.y / 2 > chunkY + VIEW_DIST)) {
                toBeUpdated.push(chunk.first);
            }
        }

        for (int dx = -VIEW_DIST; dx <= VIEW_DIST; ++dx) {
            for (int dy = -VIEW_DIST; dy <= VIEW_DIST; ++dy) {
                int i = chunkX + dx;
                int j = chunkY + dy;

                map<uint64_t, ChunkTex>::iterator it = chunk_map.find(getKey(i, j));

                if (it == chunk_map.end()) {

                    if (toBeUpdated.empty())
                        initChunk(i, j);
                    else {
                        ChunkTex ch = chunk_map.at(toBeUpdated.front());
                        chunk_map.erase(toBeUpdated.front());
                        toBeUpdated.pop();
                        updateChunk(i, j, ch);
                    }
                } else {
                    it->second.tmpFlag = true;
                }
            }
        }

        vector<uint64_t> toBeRemoved;
        for (auto &chunk : chunk_map) {
            if (!chunk.second.tmpFlag) {
                chunk.second.tex.Cleanup();
                toBeRemoved.push_back(chunk.first);
            }
        }

        for (unsigned int i = 0; i < toBeRemoved.size(); i++) {
            chunk_map.erase(toBeRemoved[i]);
        }
    }

    void cleanUp() {
        grid.Cleanup();
        water_reflection.Cleanup();
        water.Cleanup();
        perlin_tex.Cleanup();
        sky.Cleanup();
        for (auto &chunk : chunk_map) {
            chunk.second.tex.Cleanup();
        }
        chunk_map.clear();
    }

    /* ********** Helpers ********** */

  private:
    void cameraMovements(float phi, float coef) {

        if (!is_jumping && !arrows_down[UP] && !arrows_down[DOWN] && !arrows_down[RIGHT] && !arrows_down[LEFT]) {
            cam_speed *= (float)CAMERA_DECELERATION;
        }

        if (!is_jumping && !arrows_down[LEFT_UP] && !arrows_down[RIGHT_UP]) {
            cam_speed_theta *= (float)CAMERA_DECELERATION;
        }

        if ((!is_jumping && !arrows_down[LEFT_UP] && !arrows_down[RIGHT_UP])
            || cam_phi > CAM_PHI_MIN_PAD + CAM_PHI_MAX || cam_phi < CAM_PHI_MIN_PAD + CAM_PHI_MIN) {
            cam_speed_phi *= (float)CAMERA_DECELERATION;
        }

        if (!is_jumping) {
            if (arrows_down[UP]) {
                cam_speed += vecFromRot(phi, cam_theta) * (float)CAMERA_ACCELERATION;
            }
            if (arrows_down[DOWN]) {
                cam_speed -= vecFromRot(phi, cam_theta) * (float)CAMERA_ACCELERATION;
            }
            if (camera_mode == DEFAULT_CAMERA) {
                if (arrows_down[RIGHT]) {
                    cam_speed_theta += CAM_THETA_DT * (float)CAMERA_ACCELERATION;
                }
                if (arrows_down[LEFT]) {
                    cam_speed_theta -= CAM_THETA_DT * (float)CAMERA_ACCELERATION;
                }
                if (arrows_down[RIGHT_UP] && cam_phi < CAM_PHI_MIN_PAD + CAM_PHI_MAX) {
                    cam_speed_phi += CAM_PHI_DT * (float)CAMERA_ACCELERATION;
                }
                if (arrows_down[LEFT_UP] && cam_phi > CAM_PHI_MIN_PAD + CAM_PHI_MIN) {
                    cam_speed_phi -= CAM_PHI_DT * (float)CAMERA_ACCELERATION;
                }
            } else {
                if (arrows_down[RIGHT]) {
                    cam_speed -= cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, cam_theta)) * (float)CAMERA_ACCELERATION;
                }
                if (arrows_down[LEFT]) {
                    cam_speed += cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, cam_theta)) * (float)CAMERA_ACCELERATION;
                }
            }
        }

        if (length(cam_speed) > CAMERA_SPEED) {
            cam_speed = normalize(cam_speed) * (float)CAMERA_SPEED;
        }

        cam_pos += cam_speed * coef;
        cam_theta += cam_speed_theta * coef;
        cam_phi = clamp(cam_phi + cam_speed_phi * coef, CAM_PHI_MIN, CAM_PHI_MAX);
    }

    void drawChunks(mat4 &model, mat4 &view, mat4 &proj, float time, float clipping_height = 0.0f) {
        for (auto &chunk : chunk_map) {
            int i = chunk.second.x;
            int j = chunk.second.y;

            vec3 pos = vec3(chunk.second.x, 0, chunk.second.y);
            mat4 model_trans = translate(model, pos);
            grid.Draw(chunk.second.perlinBuffer_tex_id, i, j, time, model_trans, view, proj, clipping_height);
        }
    }

    void drawTrees(mat4 &model, mat4 &view, mat4 &proj, float time, float clipping_height = 0.0f) {

        mat4 PV = proj * view;
        for (auto &chunk : chunk_map) {
            int i = chunk.second.x;
            int j = chunk.second.y;
            vec3 pos = vec3(i, 0, j);

            for (unsigned int k = 0; k < chunk.second.treeList.size(); k++) {
                vec3 posInChunk = chunk.second.treeList[k].pos;
                mat4 modelForChunk = PV * translate(model, pos + posInChunk);
                float angle = 0.0f;

                for (int l = 0; l < TREE_PLANE_COUNT; l++) {
                    tree.Draw(angle, time, chunk.second.treeList[k].type, chunk.second.treeList[k].pos.y, modelForChunk,
                              clipping_height);
                    angle += (float)M_PI / TREE_PLANE_COUNT;
                }
            }
        }
    }

    inline vec3 vecFromRot(float p, float t) { return vec3(sin(p) * cos(t), cos(p), sin(p) * sin(t)); }

    inline uint64_t getKey(int i, int j) { return (((uint64_t)i) << 32) + j; }

    void initChunk(int i, int j) {
        ChunkTex chunk;
        chunk.x = i * 2;
        chunk.y = j * 2;
        chunk.tmpFlag = true;
        chunk.perlinBuffer_tex_id = chunk.tex.Init(TEX_WIDTH, TEX_HEIGHT, true, GL_RGB32F, GL_RGB);
        chunk.tex.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // (-j) because of inversion of y axis from 2D to 3D.
        perlin_tex.Draw(octave, lacunarity, fractal_increment, i - CHUNKS / 2, (-j) - CHUNKS / 2);

        // tree init
        GLfloat *perlin_tex = new GLfloat[TEX_WIDTH * TEX_HEIGHT * 3];
        glReadPixels(0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_FLOAT,
                     perlin_tex); // One fat read is much faster than small reads for all trees
        chunk.tex.Unbind();

        srand(getKey(i, j));

        int count = rand() % (MAX_TREES_PER_CHUNK / 2) + (MAX_TREES_PER_CHUNK / 2);
        for (int k = 0; k < count; k++) {

            vec3 posInChunk;
            posInChunk.x = (rand() % 1999 + 1) / 1000.0f - 1;
            posInChunk.z = (rand() % 1999 + 1) / 1000.0f - 1;

            int x = (int)((posInChunk.x + 1) * TEX_WIDTH / 2);
            int y = (int)((-posInChunk.z + 1) * TEX_HEIGHT / 2);

            posInChunk.y = perlin_tex[(x + y * TEX_HEIGHT) * 3] * 2 - 1;

            float temperature = perlin_tex[(x + y * TEX_HEIGHT) * 3 + 1];
            float altitude = perlin_tex[(x + y * TEX_HEIGHT) * 3 + 2];

            TreeStruct tree_struct;
            tree_struct.pos = posInChunk;
            tree_struct.type = NORMAL_TREE;

            int best_biome = -1;
            float min_dist = 9999;
            for (int l = 0; l < BIOME_COUNT; l++) {
                float dist = (temperature - biome_position[l].x) * (temperature - biome_position[l].x) +
                             (altitude - biome_position[l].y) * (altitude - biome_position[l].y);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_biome = l;
                }
            }

            if (biome_trees[best_biome].size() > 0) {

                float tree_chance = (rand() % 1000) / 1000.f;
                tree_struct.type = biome_trees[best_biome][rand() % biome_trees[best_biome].size()];

                if (tree_struct.pos.y < WATER_HEIGHT - 0.1) {
                    tree_struct.type = ALGAE;
                } else if (tree_struct.pos.y < WATER_HEIGHT) {
                    tree_struct.type = REED;
                } else if (tree_struct.pos.y > 0.4) {
                    tree_struct.type = SNOWY_TREE;
                }

                if (tree_chance < biome_tree_count[best_biome]) {
                    chunk.treeList.push_back(tree_struct);
                }
            }
        }

        delete[] perlin_tex;
        chunk_map.insert(pair<uint64_t, ChunkTex>(getKey(i, j), chunk));
    }

    void updateChunk(int i, int j, ChunkTex chunk) {
        chunk.x = i * 2;
        chunk.y = j * 2;
        chunk.tmpFlag = true;
        chunk.treeList.clear();
        chunk.tex.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // (-j) because of inversion of y axis from 2D to 3D.
        perlin_tex.Draw(octave, lacunarity, fractal_increment, i - CHUNKS / 2, (-j) - CHUNKS / 2);

        // tree init
        GLfloat *perlin_tex = new GLfloat[TEX_WIDTH * TEX_HEIGHT * 3];
        glReadPixels(0, 0, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_FLOAT,
                     perlin_tex); // One fat read is much faster than small reads for all trees*/

        chunk.tex.Unbind();

        srand(getKey(i, j));

        int count = rand() % (MAX_TREES_PER_CHUNK / 2) + (MAX_TREES_PER_CHUNK / 2);
        for (int k = 0; k < count; k++) {

            vec3 posInChunk;
            posInChunk.x = (rand() % 1999 + 1) / 1000.0f - 1;
            posInChunk.z = (rand() % 1999 + 1) / 1000.0f - 1;

            int x = (int)((posInChunk.x + 1) * TEX_WIDTH / 2);
            int y = (int)((-posInChunk.z + 1) * TEX_HEIGHT / 2);

            posInChunk.y = perlin_tex[(x + y * TEX_HEIGHT) * 3] * 2 - 1;

            float temperature = perlin_tex[(x + y * TEX_HEIGHT) * 3 + 1];
            float altitude = perlin_tex[(x + y * TEX_HEIGHT) * 3 + 2];

            TreeStruct tree_struct;
            tree_struct.pos = posInChunk;
            tree_struct.type = NORMAL_TREE;

            int best_biome = -1;
            float min_dist = 9999;
            for (int i = 0; i < BIOME_COUNT; i++) {
                float dist = (temperature - biome_position[i].x) * (temperature - biome_position[i].x) +
                             (altitude - biome_position[i].y) * (altitude - biome_position[i].y);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_biome = i;
                }
            }

            if (biome_trees[best_biome].size() > 0) {

                float tree_chance = (rand() % 1000) / 1000.f;
                tree_struct.type = biome_trees[best_biome][rand() % biome_trees[best_biome].size()];

                if (tree_struct.pos.y < WATER_HEIGHT - 0.1) {
                    tree_struct.type = ALGAE;
                } else if (tree_struct.pos.y < WATER_HEIGHT) {
                    tree_struct.type = REED;
                } else if (tree_struct.pos.y > 0.4) {
                    tree_struct.type = SNOWY_TREE;
                }

                if (tree_chance < biome_tree_count[best_biome]) {
                    chunk.treeList.push_back(tree_struct);
                }
            }
        }

        delete[] perlin_tex;
        chunk_map.insert(pair<uint64_t, ChunkTex>(getKey(i, j), chunk));
    }

    void set_noise_params(int doctave, float dlacunarity, float dfractal_increment) {
        octave += doctave;
        lacunarity += dlacunarity;
        fractal_increment += dfractal_increment;

        octave = clamp(octave, 1, 9);
        lacunarity = clamp(lacunarity, 0.0f, 10.0f);
        fractal_increment = clamp(fractal_increment, 1.0f, 2.0f);

        cout << "octave: " << octave << endl;
        cout << "lacunarity: " << lacunarity << endl;
        cout << "fractal increment: " << fractal_increment << endl;
    }

    /* ********** Events ********** */

  public:
    void onMouseMove(GLFWwindow *window, double x, double y) {
        (void)window;

        if (camera_mode != B_PATH) {
            cam_theta += (x - cursor_x) * MOUSE_SENSIBILTY;
            cam_phi += (y - cursor_y) * MOUSE_SENSIBILTY;
            cam_phi = clamp(cam_phi, CAM_PHI_MIN, CAM_PHI_MAX);
            cursor_x = x;
            cursor_y = y;
        }
    }

    void onResize(GLFWwindow *window) {
        glfwGetFramebufferSize(window, &window_width, &window_height);
        float ratio = window_width / (float)window_height;
        projection_matrix = perspective(45.0f, ratio, 0.1f, 100.0f);
        glViewport(0, 0, window_width, window_height);
        water_reflection.Cleanup();
        water_reflection.Init(window_width, window_height, false, GL_RGB8, GL_RGB);
    }

    void onKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
        (void)scancode, (void)mods;

        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }

        if (GLFW_KEY_1 <= key && key <= GLFW_KEY_9) {
            mode = static_cast<Mode>(key - GLFW_KEY_1);
            glfwSetInputMode(window, GLFW_CURSOR, mode == DEFAULT ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }

        if (action == GLFW_PRESS) {

            switch (key) {
            case GLFW_KEY_M:
                set_noise_params(0, 0, +0.01f);
                break;
            case GLFW_KEY_N:
                set_noise_params(0, 0, -0.01f);
                break;
            case GLFW_KEY_J:
                set_noise_params(0, +0.01f, 0);
                break;
            case GLFW_KEY_H:
                set_noise_params(0, -0.01f, 0);
                break;
            case GLFW_KEY_U:
                set_noise_params(+1, 0, 0);
                break;
            case GLFW_KEY_Z:
                set_noise_params(-1, 0, 0);
                break;
            case GLFW_KEY_K:
                if (recording) {
                    vec3 *newpos = new vec3(cam_pos);
                    path.addPoint(*newpos);
                    vec3 *new_orien = new vec3(cam_phi, cam_theta, 0);
                    cam.addPoint(*new_orien);
                }
                break;
            case GLFW_KEY_SPACE:
                if (camera_mode == GROUND && !is_jumping) {
                    is_jumping = true;
                    y_speed = (float)JUMP_SPEED;
                }
                break;
            case GLFW_KEY_R:
                camera_mode = RECORD;
                start_record = true;
                break;
            case GLFW_KEY_F:
                camera_mode = FLIGHT;
                break;
            case GLFW_KEY_P:
                camera_mode = B_PATH;
                path.addPoint(*cam_pos_first);
                cam.addPoint(*cam_rot_first);
                start_path = true;
                break;
            case GLFW_KEY_G:
                camera_mode = GROUND;
                break;
            case GLFW_KEY_B:
                camera_mode = DEFAULT_CAMERA;
                break;
            default:
                break;
            }
        }

        switch (key) {
        case GLFW_KEY_W:
            if (camera_mode == B_PATH) {
                speed.purge();
                vec3* speedvec = new vec3(curr_speed, 0, 0);
                speed.addPoint(*speedvec);
                next_speed += 0.1;
                speedvec = new vec3(next_speed, 0, 0);
                speed.addPoint(*speedvec);
                speed_changed = glfwGetTime();
            }
            arrows_down[UP] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_S:
            if (camera_mode == B_PATH) {
                speed.purge();
                vec3* speedvec = new vec3(curr_speed, 0, 0);
                speed.addPoint(*speedvec);
                next_speed -= 0.1;
                speedvec = new vec3(next_speed, 0, 0);
                speed.addPoint(*speedvec);
                speed_changed = glfwGetTime();
            }
            arrows_down[DOWN] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_D:
            arrows_down[RIGHT] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_A:
            arrows_down[LEFT] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_Q:
            arrows_down[LEFT_UP] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_E:
            arrows_down[RIGHT_UP] = (action != GLFW_RELEASE);
            break;
        default:
            break;
        }
    }
};
