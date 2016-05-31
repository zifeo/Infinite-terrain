// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Config.h"
#include "Simulation.h"
#include "icg_helper.h"

Simulation simulation;

void errorCallback(int error, const char *description) {
    fprintf(stderr, "Error %d:", error);
    fputs(description, stderr);
}

void bind_mouse(GLFWwindow *window, double x, double y) { simulation.onMouseMove(window, x, y); }

void bind_resize(GLFWwindow *window, int width, int height) {
    (void)width, (void)height;
    simulation.onResize(window);
}

void bind_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    simulation.onKey(window, key, scancode, action, mods);
}

int main(int argc, char *argv[]) {
    (void)argc, (void)argv;

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "INFINITE TERRAIN", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_NO_ERROR) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, bind_key);
    glfwSetWindowSizeCallback(window, bind_resize);
    glfwSetCursorPosCallback(window, bind_mouse);

    cout << "OpenGL " << glGetString(GL_VERSION) << endl;
    cout << "Glfw " << glfwGetVersionString() << endl;

    simulation.init(window);

    while (!glfwWindowShouldClose(window)) {
        simulation.display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    simulation.cleanUp();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
