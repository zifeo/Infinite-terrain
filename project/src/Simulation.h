#pragma once

#include "icg_helper.h"

#include "Config.h"
#include "Framebuffer.h"
#include "grid/grid.h"
#include "normalTex/normalTex.h"
#include "perlinTex/perlinTex.h"

#include <map>
#include <stdint.h>

using namespace std;
using namespace glm;

class Simulation {

private:

    bool arrows_down[4] = {false, false, false, false};
    enum { UP, DOWN, RIGHT, LEFT };

    int window_width = WINDOW_WIDTH;
    int window_height = WINDOW_HEIGHT;

    int tex_width = 1024;
    int tex_height = 1024;

    // fps
    int nb_frames = 0;
    double last_time = 0;

    // Perlin parameters
    PerlinTex perlinTex;
    int octave = 9;
    float lac = 2;
    float H = 1.25;

    // Perlin parameters are ready to be used for normal and projection
    bool perlin_ready = false;
    typedef struct {
        FrameBuffer tex;
        GLuint perlinBuffer_tex_id;
        int x;
        int y;
    } ChunkTex;
    map<long long, ChunkTex> chunkMap;

    // Normal texture
    NormalTex normalTex;
    FrameBuffer normalBuffer;

    // Final grid vue
    Grid grid;

    float phi = 2.0f;
    float theta = 0.0f;
    vec3 cam_pos = vec3(1, 1, 1);

    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 model_matrix;

public:

    /* ********** States ********** */

    void init(GLFWwindow *window) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        onResize(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        perlinTex.Init();
        grid.Init();
    }

    void display() {

        double start_time = glfwGetTime();
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view_matrix = lookAt(cam_pos, cam_pos + vecFromRot(phi, theta), vec3(0.0f, 1.0f, 0.0f));

        // cout << (cam_pos - glm::vec3(1.0f, 1.0f, 1.0f)).x << (cam_pos -
        // glm::vec3(1.0f, 1.0f, 1.0f)).y << (cam_pos - glm::vec3(1.0f, 1.0f,
        // 1.0f)).z
        // << endl;

        if (perlin_ready) {

            for (map<long long, ChunkTex>::iterator it = chunkMap.begin(); it != chunkMap.end(); ++it) {
                int i = it->second.x;
                int j = it->second.y;
                mat4 model = model_matrix *
                             translate(IDENTITY_MATRIX, vec3((i - CHUNKS / 2) * 2 - DELTA * i, 0,
                                                             -((j - CHUNKS / 2) * 2 - DELTA * j)));
                grid.Draw(it->second.perlinBuffer_tex_id, (float) start_time, model, view_matrix, projection_matrix);
            }

        } else {
            perlinTex.Draw(octave, lac, H, 0, 0);
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
            cam_pos += vecFromRot(phi, theta) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[DOWN]) {
            cam_pos -= vecFromRot(phi, theta) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[RIGHT]) {
            cam_pos -= cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, theta)) * vec3(CAMERA_SPEED);
        }
        if (arrows_down[LEFT]) {
            cam_pos += cross(vec3(0.0f, 1.0f, 0.0f), vecFromRot(phi, theta)) * vec3(CAMERA_SPEED);
        }

        // Do we need to add chunks ?
        float camX = cam_pos.x > 0 ? (cam_pos.x + 1) / 2 : (cam_pos.x + 1) / 2 - 1;
        float camY = cam_pos.z > 0 ? (cam_pos.z + 1) / 2 : (cam_pos.z + 1) / 2 - 1;

        // cout << "-------------------------" << endl;
        // cout << camX << " and " << camY << endl;
        for (int dx = -VIEW_DIST; dx <= VIEW_DIST; ++dx) {
            for (int dy = -VIEW_DIST; dy <= VIEW_DIST; ++dy) {
                float i = camX + dx + 1;
                float j = -camY + dy + 1;

                // cout << i << " - " << j << endl;

                if (dx * dx + dy * dy <= VIEW_DIST * VIEW_DIST) {

                    map<long long, ChunkTex>::iterator it = chunkMap.find(getKey(i, j));

                    if (it == chunkMap.end()) { // no element at this position
                        initChunk(i, j);
                        cout << "Added chunk " << i << "-" << j << endl;
                    }
                }
            }
        }

        // Do we need to remove chunks ?
        vector<long long> toBeRemoved;
        for (map<long long, ChunkTex>::iterator it = chunkMap.begin(); it != chunkMap.end(); ++it) {
            int i = it->second.x;
            int j = it->second.y;

            int dx = -camX - 1 + i;
            int dy = camY - 1 + j;

            if (dx * dx + dy * dy > VIEW_DIST * VIEW_DIST * 1.1) {
                it->second.tex.Cleanup();
                toBeRemoved.push_back(it->first);
                cout << "Removed chunk " << i << "-" << j << endl;
            }
        }

        for (unsigned int i = 0; i < toBeRemoved.size(); i++) {
            chunkMap.erase(toBeRemoved[i]);
        }
    }

    void cleanUp() {
        grid.Cleanup();
        normalTex.Cleanup();
        perlinTex.Cleanup();
        normalBuffer.Cleanup();

        for (map<long long, ChunkTex>::iterator it = chunkMap.begin(); it != chunkMap.end(); ++it) {
            it->second.tex.Cleanup();
        }
    }

    /* ********** Helpers ********** */

    inline vec3 vecFromRot(float p, float t) {
        return vec3(sin(p) * cos(t), cos(p), sin(p) * sin(t));
    }

    inline long long getKey(int i, int j) {
        return ((i + 1000) % 1000) * 1000 + (j + 1000) % 1000;
    }

    void initChunk(int i, int j) {
        ChunkTex chunk;
        chunk.x = i;
        chunk.y = j;
        chunk.perlinBuffer_tex_id = chunk.tex.Init(tex_width, tex_height, true);
        chunk.tex.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        perlinTex.Draw(octave, lac, H, i - CHUNKS / 2, j - CHUNKS / 2);
        chunk.tex.Unbind();
        chunkMap.insert(pair<long long, ChunkTex>(getKey(i, j), chunk));
    }

    /* ********** Events ********** */

    void onMouseMove(GLFWwindow *window, double x, double y) {

        int half_width = 0, half_height = 0;
        glfwGetWindowSize(window, &half_width, &half_height);
        //cout << half_width << ":" << half_height << endl;
        half_width /= 2;
        half_height /= 2;

        int dx = (int)x - half_width;
        int dy = (int)y - half_height;
        //cout << dx << ":" << dy << endl;

        theta += dx * MOUSE_SENSIBILTY;
        phi += dy * MOUSE_SENSIBILTY;
        phi = clamp(phi, (float)M_PI / 10, 9 * (float)M_PI / 10);

        if (dx != 0 || dy != 0) {
            glfwSetCursorPos(window, half_width, half_height - FULLHDFTW);
        }
    }

    void onResize(GLFWwindow *window) {
        glfwGetFramebufferSize(window, &window_width, &window_height);
        cout << "resize to " << window_width << " x " << window_height << endl;

        float ratio = 1.0f * window_width / window_height;
        projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
        glViewport(0, 0, window_width, window_height);
    }

    void onKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {

            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }

            if (GLFW_KEY_1 <= key && key <= GLFW_KEY_9) {
                octave = key - GLFW_KEY_0;
                cout << "nbr octave : " << octave << endl;
            }

            switch(key) {

                case GLFW_KEY_Q:
                    H += 0.01;
                    H = H > 2 ? 2 : H;
                    cout << "new H" << H << endl;
                    break;
                case GLFW_KEY_W:
                    H -= 0.01;
                    H = H < -2 ? -2 : H;
                    cout << "new H" << H << endl;
                    break;
                case GLFW_KEY_A:
                    lac += 0.01;
                    lac = lac > 10 ? 10 : lac;
                    cout << "new lac" << lac << endl;
                    break;
                case GLFW_KEY_S:
                    lac -= 0.05;
                    lac = lac < 0 ? 0 : lac;
                    cout << "new lac" << lac << endl;
                    break;
                case GLFW_KEY_D:
                    perlin_ready = !perlin_ready;
                    break;

                default:
                    break;

            }
        }

        switch(key) {
            case GLFW_KEY_UP:
                arrows_down[UP] = (action != GLFW_RELEASE);
                break;
            case GLFW_KEY_DOWN:
                arrows_down[DOWN] = (action != GLFW_RELEASE);
                break;
            case GLFW_KEY_RIGHT:
                arrows_down[RIGHT] = (action != GLFW_RELEASE);
                break;
            case GLFW_KEY_LEFT:
                arrows_down[LEFT] = (action != GLFW_RELEASE);
                break;
            default:
                break;
        }
    }

};
