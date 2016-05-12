#pragma once

#include "icg_helper.h"

#include "Config.h"
#include "Framebuffer.h"
#include "grid/grid.h"
#include "perlinTex/perlinTex.h"
#include "sky/sky.h"
#include "tree/tree.h"
#include "water/water.h"

#include <map>
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
    enum { UP = 0, DOWN, RIGHT, LEFT };
    bool arrows_down[4] = {false, false, false, false};
    float camera_phi = 2.0f;
    float camera_theta = 0.0f;
    vec3 cam_pos = vec3(0, 1, 0);

    // MVP
    mat4 projection_matrix;
    mat4 model_matrix;
    mat4 view_matrix;

    // view mode
    enum Mode { DEFAULT = 0, PERLIN, TEXTURE };
    Mode mode = DEFAULT;

    // fps
    int nb_frames = 0;
    double last_time = 0;

    // Perlin parameters
    PerlinTex perlinTex;
    int octave = 9;
    float lacunarity = 2;
    float fractal_increment = 1.25;

    // chunks
    typedef struct {
        FrameBuffer tex;
        GLuint perlinBuffer_tex_id;
        int x;
        int y;
        std::vector<vec2> treeList;
    } ChunkTex;

    map<long, ChunkTex> chunk_map;

    // objects
    Grid grid;
    Water water;
    Tree tree;
    Sky sky;

    // TODO : normal tex ?

    // water reflection
    FrameBuffer water_reflection;

  public:
    /* ********** States ********** */

    void init(GLFWwindow *window) {

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        onResize(window);

        perlinTex.Init();
        grid.Init();
        GLuint water_reflection_tex_id = water_reflection.Init(window_width, window_height);
        water.Init(water_reflection_tex_id);
        sky.Init();
        tree.Init();
    }

    void drawChunk(mat4 view) {
        for (auto &chunk : chunk_map) {
            int i = chunk.second.x;
            int j = chunk.second.y;

            vec3 pos = vec3(i, 0, j);
            mat4 model = translate(model_matrix, pos);
            grid.Draw(chunk.second.perlinBuffer_tex_id, i, j, model, view, projection_matrix);
        }
    }

    void display() {

        double start_time = glfwGetTime();
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view_matrix = lookAt(cam_pos, cam_pos + vecFromRot(camera_phi, camera_theta), vec3(0.0f, 1.0f, 0.0f));
        vec3 cam_pos2 = vec3(0, -1, 0);
        mat4 view_matrix_reflection = lookAt(cam_pos2, cam_pos2 + vecFromRot(camera_phi + M_PI, camera_theta), vec3(0.0f, 1.0f, 0.0f));

        switch (mode) {

        case DEFAULT:

            break;

        case PERLIN:
            perlinTex.Draw(octave, lacunarity, fractal_increment, 0, 0);
            break;
        case TEXTURE:

            water_reflection.Bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            drawChunk(view_matrix_reflection);
            water_reflection.Unbind();

            drawChunk(view_matrix);

            for (auto &chunk : chunk_map) {
                int i = chunk.second.x;
                int j = chunk.second.y;

                vec3 pos = vec3(i, 0, j);
                mat4 model = translate(model_matrix, pos);
                water.Draw((float)start_time, i, j, model, view_matrix, projection_matrix);
            }

            /*
            for (auto &chunk : chunk_map) {
                int i = chunk.second.x;
                int j = chunk.second.y;

                for (int k = 0; k < chunk.second.treeList.size(); k++) {
                    vec2 posInChunk = chunk.second.treeList[k];
                    mat4 model = translate(model_matrix, vec3((i - CHUNKS / 2) * 2 - DELTA * i + posInChunk.x, 0,
                                                              -((j - CHUNKS / 2) * 2 - DELTA * j + posInChunk.y)));

                    if (TURNING_TREES) {
                        float x = (i - CHUNKS / 2) * 2 - DELTA * i + posInChunk.x - cam_pos.x;
                        float y = -((j - CHUNKS / 2) * 2 - DELTA * j + posInChunk.y) - cam_pos.z;
                        float angle = y > 0 ? M_PI / 2 + acos(x / (sqrt(x * x + y * y)))
                                            : M_PI / 2 - acos(x / (sqrt(x * x + y * y)));
                        tree.Draw(angle, posInChunk, it->second.perlinBuffer_tex_id, (float)start_time,
                                  it->second.perlinBuffer_tex_id, 0, model, view_matrix, projection_matrix);
                    } else {
                        float angle = 0.0f;
                        for (int l = 0; l < TREE_PLANE_COUNT; l++) {
                            tree.Draw(angle, posInChunk, it->second.perlinBuffer_tex_id, (float)start_time,
                                      it->second.perlinBuffer_tex_id, 0, model, view_matrix, projection_matrix);
                            angle += M_PI / TREE_PLANE_COUNT;
                        }
                    }
                }
            }
             */

            sky.Draw(translate(projection_matrix * model_matrix * view_matrix, cam_pos));
            break;
        }

        // Measure speed
        ++nb_frames;
        if (start_time - last_time >= 1.0) { // over 1 second
            cout << nb_frames << " frames" << endl;
            nb_frames = 0;
            last_time = start_time;
        }

        // Camera movements
        if (arrows_down[UP]) {
            cam_pos += vecFromRot(camera_phi, camera_theta) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[DOWN]) {
            cam_pos -= vecFromRot(camera_phi, camera_theta) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[RIGHT]) {
            cam_pos -= cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(camera_phi, camera_theta)) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[LEFT]) {
            cam_pos += cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(camera_phi, camera_theta)) * vec3(CAMERA_SPEED);
        }

        // + 1 is because we are in the middle of a chunk
        // / 2 because a chunk is of length 2
        int chunkX = floor((cam_pos.x + 1) / 2);
        int chunkY = floor((cam_pos.z + 1) / 2);

        for (int dx = -VIEW_DIST; dx <= VIEW_DIST; ++dx) {
            for (int dy = -VIEW_DIST; dy <= VIEW_DIST; ++dy) {
                int i = chunkX + dx;
                int j = chunkY + dy;

                map<long, ChunkTex>::iterator it = chunk_map.find(getKey(i, j));

                if (it == chunk_map.end()) {
                    initChunk(i, j);
                }
            }
        }

        // TODO : cleanup ?
    }

    void cleanUp() {
        grid.Cleanup();
        water_reflection.Cleanup();
        water.Cleanup();
        perlinTex.Cleanup();
        sky.Cleanup();
        for (auto &chunk : chunk_map) {
            chunk.second.tex.Cleanup();
        }
        chunk_map.clear();
    }

    /* ********** Helpers ********** */

    inline vec3 vecFromRot(float p, float t) { return vec3(sin(p) * cos(t), cos(p), sin(p) * sin(t)); }

    inline long getKey(int i, int j) { return (i << sizeof(long) / 2) + j; }

    void initChunk(int i, int j) {
        ChunkTex chunk;
        chunk.x = i * 2;
        chunk.y = j * 2;
        chunk.perlinBuffer_tex_id = chunk.tex.Init(TEX_WIDTH, TEX_HEIGHT, true);
        chunk.tex.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // (-j) because of inversion of y axis from 2D to 3D.
        perlinTex.Draw(octave, lacunarity, fractal_increment, i - CHUNKS / 2, (-j) - CHUNKS / 2);
        chunk.tex.Unbind();

        // tree init
        int count = rand() % (MAX_TREES_PER_CHUNK / 2) + (MAX_TREES_PER_CHUNK / 2);
        for (int k = 0; k < count; k++) {

            vec2 posInChunk;
            posInChunk.x = (rand() % 2000) / 1000.0f - 1;
            posInChunk.y = (rand() % 2000) / 1000.0f - 1;

            chunk.treeList.push_back(posInChunk);
        }

        chunk_map.insert(pair<long, ChunkTex>(getKey(i, j), chunk));
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

    void onMouseMove(GLFWwindow *window, double x, double y) {
        camera_theta += (x - cursor_x) * MOUSE_SENSIBILTY;
        camera_phi += (y - cursor_y) * MOUSE_SENSIBILTY;
        camera_phi = clamp(camera_phi, (float)(M_PI / 10), (float)(9 * M_PI / 10));
        cursor_x = x;
        cursor_y = y;
    }

    void onResize(GLFWwindow *window) {
        glfwGetFramebufferSize(window, &window_width, &window_height);
        float ratio = window_width / (float)window_height;
        projection_matrix = perspective(45.0f, ratio, 0.1f, 100.0f);
        glViewport(0, 0, window_width, window_height);
        water_reflection.Cleanup();
        water_reflection.Init(window_width, window_height);
    }

    void onKey(GLFWwindow *window, int key, int scancode, int action, int mods) {

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
            case GLFW_KEY_O:
                set_noise_params(0, 0, +0.01f);
                break;
            case GLFW_KEY_P:
                set_noise_params(0, 0, -0.01f);
                break;
            case GLFW_KEY_F:
                set_noise_params(0, +0.01f, 0);
                break;
            case GLFW_KEY_G:
                set_noise_params(0, -0.01f, 0);
                break;
            case GLFW_KEY_L:
                set_noise_params(+1, 0, 0);
                break;
            case 59 /*É*/:
                set_noise_params(-1, 0.01, 0);
                break;
            default:
                break;
            }
        }

        switch (key) {
        case GLFW_KEY_W:
            arrows_down[UP] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_S:
            arrows_down[DOWN] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_D:
            arrows_down[RIGHT] = (action != GLFW_RELEASE);
            break;
        case GLFW_KEY_A:
            arrows_down[LEFT] = (action != GLFW_RELEASE);
            break;
        default:
            break;
        }
    }
};
