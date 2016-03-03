// glew must be before glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// contains helper functions such as shader compiler
#include "icg_helper.h"

// vertex position of the triangle
const GLfloat triangle_vertex_positions[] = {-1.0f, -1.0f, 0.0f,
                                             1.0f, -1.0f, 0.0f,
                                             0.0f,  1.0f, 0.0f};

void Init() {
    // sets background color
    glClearColor(0.937, 0.937, 0.937 /*gray*/, 1.0 /*solid*/);
    
    // compile the shaders
    GLuint program_id = icg_helper::LoadShaders("helloworld_vshader.glsl",
                                                "helloworld_fshader.glsl");
    if(!program_id) {
        exit(EXIT_FAILURE);
    }

    glUseProgram(program_id);
    
    // setup vertex array;
    // vertex arrays wrap buffers & attributes together
    // creating it is mandatory in newer OpenGL versions (>= 3.0)
    GLuint vertex_array_id;
    glGenVertexArrays(ONE, &vertex_array_id);
    glBindVertexArray(vertex_array_id);
    
    // generate memory for vertex buffer
    GLuint vertex_buffer;
    glGenBuffers(ONE, &vertex_buffer);
    // the subsequent commands will affect the specified buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // pass the vertex positions to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertex_positions),
                 triangle_vertex_positions, GL_STATIC_DRAW);

    // creates Vertex Attribute to store Vertex Positions
    GLuint vertex_point_id = glGetAttribLocation(program_id, "vpoint");
    glEnableVertexAttribArray(vertex_point_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                          ZERO_STRIDE, ZERO_BUFFER_OFFSET);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0 /*buffer offset*/, 3 /*#vertices*/);
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void KeyCallback(GLFWwindow* window, int key, int scancode,
                 int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
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
    GLFWwindow* window = glfwCreateWindow(512, 512, "hello world", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // makes the OpenGL context of window current on the calling thread
    glfwMakeContextCurrent(window);

    // set the callback for keyboard keys
    glfwSetKeyCallback(window, KeyCallback);

    // GLEW Initialization (must have a context)
    // https://www.opengl.org/wiki/OpenGL_Loading_Library
    glewExperimental = GL_TRUE; // fixes glew error (see above link)
    if(glewInit() != GLEW_NO_ERROR) {
        fprintf(stderr, "Failed to initialize GLEW\n");
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

    // close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
