#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "icg_helper.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Triangle {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer

    // vertex position of the triangle
    GLfloat vpoint[9] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f
    };

public:
    void Init() {
        // LoadShaders                  <--- compile the shaders
        // glUseProgram                 <--- bind the shader (for attributes)
        //---
        // glGenVertexArrays            <--- create vertex array
        // glBindVertexArray            <--- and bind it
        //---
        // glGenBuffers                 <--- generate buffer pointer
        // glBindBuffers                <--- make it current
        // glBufferData                 <--- tell it where to find data
        //---
        // glGetAttribLocation          <--- fetch attribute ID
        // glEnableVertexAttribArray    <--- make it the current
        // glVertexAttribPointer        <--- specify layout of attribute
        //---
        // stbi_load                    <--- load image from file
        // glGenTextures                <--- create texture buffer
        // glBindTexture                <--- and bind it
        // glTexImage2D                 <--- fill the texture with image
        // stbi_image_free              <--- free the image in cpu memory
        //---
        // glGetUniformLocation         <--- fetch ID from shader
        // glUniform*(...)              <--- set the values of a uniform
        //--- To avoid resource pollution, unload resources
        // glUseProgram(0)              <--- unbind program (safety!)
        // glBindVertexArray(0)         <--- unbind array (safety!)

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("triangle_vshader.glsl",
                                              "triangle_fshader.glsl");
        if(!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // Vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // Vertex Buffer
        glGenBuffers(1, &vertex_buffer_object_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

        // position attribute, fetch Attribute ID for Vertex Positions
        GLuint position = glGetAttribLocation(program_id_, "vpoint");
        glEnableVertexAttribArray(position); // enable it
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE,
                              ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        const float alpha = M_PI_4;
        const float tx = 0.5;
        const float ty = 0.5;
        const float sx = 0.25;
        const float sy = 0.25;
        glm::mat4 I = glm::mat4(1.0f);
        glm::mat4 T = glm::translate(I, glm::vec3(tx, ty, 0));
        glm::mat4 S = glm::scale(T, glm::vec3(sx, sy, 0));
        glm::mat4 R = glm::rotate(S, alpha, glm::vec3(0, 0, 1));
        GLuint M_id = glGetUniformLocation(program_id_, "M");
        glUniformMatrix4fv(M_id, 1, GL_FALSE, glm::value_ptr(R));
    }

    void Cleanup() {
        // glDelete*(...);
    }

    void Draw() {
        // glUseProgram(_pid);
        // glBindVertexArray(_vao);
        //--- if there is a texture :
        // glActiveTexture          <--- enable certain tex-unit
        // glBindTexture            <--- bind it to CPU memory
        //--- if there a "changing" value to pass to the shader :
        // glUniform*(...)          <--- set the values of a uniform
        //---
        // glDraw*(...)             <--- draws the buffers
        //---
        // glBindVertexArray(0);
        // glUseProgram(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};
