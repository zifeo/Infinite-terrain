// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "framebuffer.h"
#include "trackball.h"

#include "grid/grid.h"
#include "perlinTex/perlinTex.h"
#include "normalTex/normalTex.h"

#define CHUNKS 10

int window_width = 1280;
int window_height = 720;
int tex_width = 1024;
int tex_height = 1024;
int nbFrames = 0;
float lastTime = 0;
float save_y = 0;

// Perlin parameters
PerlinTex perlinTex;
int octave = 9;
float lac = 2;
float H = 1.25;
// Perlin parameters are ready to be used for normal and projection
bool perlin_ready = false;
FrameBuffer texs[CHUNKS];
GLuint perlinBuffer_tex_id[CHUNKS];

// Normal texture
NormalTex normalTex;
FrameBuffer normalBuffer;

// Final grid vue
Grid grid;

// Trackball
// Helper matrices
mat4 projection_matrix;
mat4 view_matrix;
mat4 trackball_matrix;
mat4 old_trackball_matrix;
mat4 quad_model_matrix;

Trackball trackball;

using namespace glm;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // Trackball matrices initial setup
    quad_model_matrix = IDENTITY_MATRIX;
    trackball_matrix = IDENTITY_MATRIX;


    // All texture, vues and framebuffer init
    perlinTex.Init();

    grid.Init();

}

void frameBufferInit() {
    for (int i = 0; i < CHUNKS; i++) {
        perlinBuffer_tex_id[i] = texs[i].Init(tex_width, tex_height, true);
        texs[i].Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        perlinTex.Draw(octave, lac, H, i, 0);
        texs[i].Unbind();
    }
}

void Display() {
    double currentTime = glfwGetTime();
    glViewport(0,0,window_width,window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (perlin_ready) {

        glViewport(0,0,window_width,window_height);

        for (int i = 0; i < CHUNKS; i++) {
            mat4 model = quad_model_matrix * translate(IDENTITY_MATRIX, vec3(i*2,0,0));
            grid.Draw(perlinBuffer_tex_id[i], currentTime, trackball_matrix * model, view_matrix, projection_matrix);
        }
    } else {
        perlinTex.Draw(octave, lac, H, 0, 0);
    }

    // Measure speed
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 || lastTime == 0 ){ // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%d frame\n", nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }
}

// transforms glfw screen coordinates into normalized OpenGL coordinates.
vec2 TransformScreenCoords(GLFWwindow* window, int x, int y) {
    // the framebuffer and the window doesn't necessarily have the same size
    // i.e. hidpi screens. so we need to get the correct one
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    return vec2(2.0f * (float)x / width - 1.0f,
                1.0f - 2.0f * (float)y / height);
}

void MouseButton(GLFWwindow* window, int button, int action, int mod) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        trackball.BeingDrag(p.x, p.y);
        old_trackball_matrix = trackball_matrix;
        // Store the current state of the model matrix.
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double x_i, y_i;
        glfwGetCursorPos(window, &x_i, &y_i);
        vec2 p = TransformScreenCoords(window, x_i, y_i);
        save_y = p.y;
    }
}

void MousePos(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        vec2 p = TransformScreenCoords(window, x, y);
        // TODO 3: Calculate 'trackball_matrix' given the return value of
        // trackball.Drag(...) and the value stored in 'old_trackball_matrix'.
        // See also the mouse_button(...) function.
        trackball_matrix = trackball.Drag(p.x, p.y) * old_trackball_matrix;
    }

    // zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        // TODO 4: Implement zooming. When the right mouse button is pressed,
        // moving the mouse cursor up and down (along the screen's y axis)
        // should zoom out and it. For that you have to update the current
        // 'view_matrix' with a translation along the z axis.
        vec2 p = TransformScreenCoords(window, x, y);
        view_matrix = translate(view_matrix, vec3(0, 0, 2*(p.y - save_y)));
        save_y = p.y;
    }
}

// Gets called when the windows/framebuffer is resized.
void resize_callback(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &window_width, &window_height);

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
    glViewport(0, 0, window_width, window_height);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
        octave = key - GLFW_KEY_0;
        cout << "nbr octave : " << octave << endl;
    } else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        H += 0.01;
        H = H > 2 ? 2 : H;
        cout << "new H" << H << endl;
    } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        H -= 0.01;
        H = H < -2 ? -2 : H;
        cout << "new H" << H << endl;
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        lac += 0.01;
        lac = lac > 10 ? 10 : lac;
        cout << "new lac" << lac << endl;
    } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        lac -= 0.05;
        lac = lac < 0 ? 0 : lac;
        cout << "new lac" << lac << endl;
    } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        perlin_ready = !perlin_ready;
    }
}

int main(int argc, char *argv[]) {
    /// GLFW Initialization
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return EXIT_FAILURE;
    }
    glfwSetErrorCallback(ErrorCallback);

    /// Hint GLFW that we would like an OpenGL 3 context (at least)
    /// http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /// Attempt to open the window: fails if required version unavailable
    /// @note some Intel GPUs do not support OpenGL 3.2
    /// @note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "mirror_floor", NULL, NULL);
    if( !window ){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /// Link the current context to the window we created
    glfwMakeContextCurrent(window);

    /// Set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    glfwSetFramebufferSizeCallback(window, resize_callback);

    // set the mouse press and position callback
    glfwSetMouseButtonCallback(window, MouseButton);
    glfwSetCursorPosCallback(window, MousePos);

    /// GLEW Initialization (must have a context)
    glewExperimental = GL_TRUE; ///<
    if( glewInit() != GLEW_NO_ERROR ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
    Init(window);
    KeyCallback(window, GLFW_KEY_KP_1, 0, 0, 0);

    // update the window size with the framebuffer size (on hidpi screens the
    // framebuffer is bigger)
    glfwGetFramebufferSize(window, &window_width, &window_height);
    resize_callback(window, window_width, window_height);

    frameBufferInit();

    /// Render loop & keyboard input
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    grid.Cleanup();
    normalTex.Cleanup();
    perlinTex.Cleanup();
    normalBuffer.Cleanup();

    for (int i = 0; i < CHUNKS; i++) {
        texs[i].Cleanup();
    }

    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit( EXIT_SUCCESS );
}
