// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "framebuffer.h"

#include "floor/floor.h"
#include "cube/cube.h"
#include "perlinTex/perlinTex.h"

int window_width = 1280;
int window_height = 720;
int nbFrames = 0;
float lastTime = 0;


Cube cube;
Floor shinyfloor;

// Perlin parameters
PerlinTex perlinTex;
FrameBuffer perlinFramebuffer;
int octave = 4;
float lac = 0.9;
float H = 0.3;
// Perlin parameters are ready to be used for normal and projection
bool perlin_ready = false;

// Normal texture
NormalTex normalTex;
FrameBuffer normalBuffer;


using namespace glm;

mat4 projection_matrix;

void Init(GLFWwindow* window) {
    glClearColor(1.0, 1.0, 1.0 /*white*/, 1.0 /*solid*/);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);

    /*cube.Init();
    // TODO: initialize framebuffer
    GLuint framebuffer_texture_id = framebuffer.Init(window_width, window_height);
    // TODO: initialize shinyfloor with the FB texture
    shinyfloor.Init(framebuffer_texture_id);*/



    perlinTex.Init();
}

void Display() {
    glViewport(0,0,window_width,window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    vec3 cam_pos(2.0f, 2.0f, -2.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 0.0f, -1.0f);
    mat4 view = lookAt(cam_pos, cam_look, cam_up);
    mat4 view_projection = projection_matrix * view;

    // TODO: mirror the camera position
    // TODO: create new VP for mirrored camera
    // TODO: render the cube using the mirrored camera
    // HINT: this render will be done in the framebuffer texture (remember bind/unbind)

    framebuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.Draw(view_projection);
    framebuffer.Unbind();

    cam_pos = vec3(2.0f, 2.0f, 2.0f);
    cam_up = vec3(0.0f, 0.0f, 1.0f);
    view = lookAt(cam_pos, cam_look, cam_up);
    view_projection = projection_matrix * view;
    
    shinyfloor.Draw(view_projection);
    cube.Draw(view_projection);
     */

    if (perlin_ready) {
        perlinFramebuffer.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    perlinTex.Draw(octave, lac, H);
    if (perlin_ready) {
        perlinFramebuffer.Unbind();
    }

    if (perlin_ready) {

    }

    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 || lastTime == 0 ){ // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%d frame\n", nbFrames);
        nbFrames = 0;
        lastTime += 1.0;
    }
}

// Gets called when the windows/framebuffer is resized.
void resize_callback(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &window_width, &window_height);
    float ratio = window_width / (float) window_height;
    projection_matrix = perspective(45.0f, ratio, 0.1f, 10.0f);
    glViewport(0, 0, window_width, window_height);
    perlinFramebuffer.Cleanup();
    perlinFramebuffer.Init(window_width, window_height);
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
        H = H > 1 ? 1 : H;
        cout << "new H" << H << endl;
    } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        H -= 0.01;
        H = H < 0 ? 0 : H;
        cout << "new H" << H << endl;
    } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        lac += 0.05;
        lac = lac > 10 ? 1 : lac;
        cout << "new lac" << lac << endl;
    } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        lac -= 0.05;
        lac = lac < 0 ? 0 : lac;
        cout << "new lac" << lac << endl;
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

    /// GLEW Initialization (must have a context)
    glewExperimental = GL_TRUE; ///<
    if( glewInit() != GLEW_NO_ERROR ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;
    Init(window);
    KeyCallback(window, GLFW_KEY_KP_1, 0, 0, 0);

    /// Render loop & keyboard input
    while(!glfwWindowShouldClose(window)){
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit( EXIT_SUCCESS );
}
