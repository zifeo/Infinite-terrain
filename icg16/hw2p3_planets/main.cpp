// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#include "quad/quad.h"

Quad sun;
Quad earth;
Quad moon;
const float speed = 0.5;

void Init() {
    // sets background color
    glClearColor(1.0,1.0,1.0 /*white*/, 1.0 /*solid*/);

    sun.Init("sun.tga");
    earth.Init("earth.tga");
    moon.Init("moon.tga");
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time_s = glfwGetTime() * speed;

    const float alphaSun = time_s * 0.5;
    const float txSun = 0.5;
    const float tySun = 0;
    const float sxSun = 0.2;
    const float sySun = 0.2;
    glm::mat4 ISun = glm::mat4(1.0f);
    glm::mat4 TSun = glm::translate(ISun, glm::vec3(txSun, tySun, 0));
    glm::mat4 SSun = glm::scale(TSun, glm::vec3(sxSun, sySun, 0));
    glm::mat4 RSun = glm::rotate(SSun, alphaSun, glm::vec3(0, 0, 1));
    glm::mat4 modelSun = RSun;
    sun.Draw(modelSun);

    const float alphaEarth = time_s;
    const float txEarth = (txSun * 2) * -cos(time_s);
    const float tyEarth = 0.5 * sin(time_s);
    const float sxEarth = 0.1;
    const float syEarth = 0.1;
    glm::mat4 IEarth = glm::mat4(1.0f);
    glm::mat4 TEarth = glm::translate(IEarth, glm::vec3(txEarth, tyEarth, 0));
    glm::mat4 SEarth = glm::scale(TEarth, glm::vec3(sxEarth, syEarth, 0));
    glm::mat4 REarth = glm::rotate(SEarth, alphaEarth, glm::vec3(0, 0, 1));
    glm::mat4 modelEarth = REarth;
    earth.Draw(modelEarth);

    const float alphaMoon = time_s;
    const float txMoon = txEarth + 0.2 * cos(time_s * 4);
    const float tyMoon = tyEarth + 0.2 * sin(time_s * 4);
    const float sxMoon = 0.05;
    const float syMoon = 0.05;
    glm::mat4 IMoon = glm::mat4(1.0f);
    glm::mat4 TMoon = glm::translate(IMoon, glm::vec3(txMoon, tyMoon, 0));
    glm::mat4 SMoon = glm::scale(TMoon, glm::vec3(sxMoon, syMoon, 0));
    glm::mat4 RMoon = glm::rotate(SMoon, alphaMoon, glm::vec3(0, 0, 1));
    glm::mat4 modelMoon = RMoon;
    moon.Draw(modelMoon);

    // compute the transformation matrices
    // {stuff}.Draw({stuff}_modelmatrix);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char *argv[]) {
    // GLFW Initialization
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(ErrorCallback);

    // hint GLFW that we would like an OpenGL 3 context (at least)
    // http://www.glfw.org/faq.html#how-do-i-create-an-opengl-30-context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // attempt to open the window: fails if required version unavailable
    // note some Intel GPUs do not support OpenGL 3.2
    // note update the driver of your graphic card
    GLFWwindow* window = glfwCreateWindow(512, 512, "planets", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for escape key
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf( stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }

    cout << "OpenGL" << glGetString(GL_VERSION) << endl;

    // initialize our OpenGL program
    Init();

    // render loop
    while(!glfwWindowShouldClose(window)) {
        Display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    sun.Cleanup();
    earth.Cleanup();
    moon.Cleanup();
    // {stuff}.Cleanup()

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
