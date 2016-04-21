#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "icg_helper.h"

class NormalTex {

  private:
    GLuint vertex_array_id_;      // vertex array object
    GLuint program_id_;           // GLSL shader program ID
    GLuint vertex_buffer_object_; // memory buffer
    GLuint texture_id_;           // texture ID

  public:
    void Init(GLuint tex_perlin) {

        // compile the shaders
        program_id_ = icg_helper::LoadShaders("normalTex_vshader.glsl", "normalTex_fshader.glsl");

        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex Array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            const GLfloat vertex_point[] = {/*V1*/ -1.0f, -1.0f, 0.0f,
                                            /*V2*/ +1.0f, -1.0f, 0.0f,
                                            /*V3*/ -1.0f, +1.0f, 0.0f,
                                            /*V4*/ +1.0f, +1.0f, 0.0f};
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point), vertex_point, GL_STATIC_DRAW);

            // attribute
            GLint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {
            const GLfloat vertex_texture_coordinates[] = {/*V1*/ 0.0f, 0.0f,
                                                          /*V2*/ 1.0f, 0.0f,
                                                          /*V3*/ 0.0f, 1.0f,
                                                          /*V4*/ 1.0f, 1.0f};

            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                         vertex_texture_coordinates, GL_STATIC_DRAW);

            // attribute
            GLint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // bind 2D texture (perlin noise)
        {
            texture_id_ = tex_perlin;
            GLuint tex_perlin_id = glGetUniformLocation(program_id_, "tex_perlin");
            glUniform1i(tex_perlin_id, 0 /*GL_TEXTURE0*/);
        }

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
    }

    void Draw() {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
