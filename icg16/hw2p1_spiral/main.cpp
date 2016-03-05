// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

#define N 400

#include <glm/gtc/matrix_transform.hpp>

#include "triangle/triangle.h"

Triangle triangle;

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);

    triangle.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //Fermat's spiral
    /*for (int i = 0; i < N; i++) {
        float theta = i * 137.508;
        float r = 0.003 * sqrt(theta);

        const float alpha = 0;
        const float tx = r * cos(theta);
        const float ty = r * sin(theta);
        const float sx = 0.02;
        const float sy = 0.02;
        glm::mat4 I = glm::mat4(1.0f);
        glm::mat4 T = glm::translate(I, glm::vec3(tx, ty, 0));
        glm::mat4 S = glm::scale(T, glm::vec3(sx, sy, 0));
        glm::mat4 R = glm::rotate(S, -theta, glm::vec3(0, 0, 1));
        glm::mat4 model = R;
        triangle.Draw(model);
    }*/
    for (int i = 0; i < 100; i++) {
        float theta = i * 16 / 180.0 * 3.1415926535;
        float r = 0.009 * i;

        const float alpha = 0;
        const float tx = r * cos(theta);
        const float ty = r * sin(theta);
        const float sx = 0.05 * r;
        const float sy = 0.05 * r;
        glm::mat4 I = glm::mat4(1.0f);
        glm::mat4 T = glm::translate(I, glm::vec3(tx, ty, 0));
        glm::mat4 S = glm::scale(T, glm::vec3(sx, sy, 0));
        glm::mat4 R = glm::rotate(S, -theta, glm::vec3(0, 0, 1));
        glm::mat4 model = R;
        triangle.Draw(model);
    }
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "spiral", NULL, NULL);
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

    triangle.Cleanup();

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

