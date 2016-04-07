#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"


// Perlin utilities
static const GLint permutaionArray[256] = {
        252, 58, 52, 190, 63, 235, 229, 24, 216, 120, 240, 110, 90, 132, 133, 92, 162, 57, 179, 38, 101, 6,
        29, 21, 214, 97, 164, 10, 142, 231, 75, 111, 192, 106, 95, 236, 143, 150, 188, 138, 42, 8, 211, 51,
        47, 62, 233, 154, 145, 196, 115, 13, 238, 114, 147, 98, 170, 94, 168, 121, 215, 183, 66, 152, 65,
        34, 88, 209, 85, 59, 226, 37, 31, 12, 253, 230, 244, 200, 210, 43, 68, 16, 177, 89, 202, 141, 84,
        48, 172, 7, 53, 223, 131, 129, 180, 255, 158, 203, 36, 74, 241, 69, 80, 225, 220, 112, 27, 189, 185,
        91, 117, 9, 128, 221, 187, 165, 206, 41, 70, 144, 86, 217, 222, 67, 116, 136, 167, 15, 76, 243, 96,
        0, 122, 18, 234, 251, 250, 212, 14, 64, 54, 181, 137, 194, 77, 108, 107, 30, 159, 184, 26, 171, 139,
        113, 174, 60, 195, 35, 239, 44, 87, 3, 124, 71, 73, 242, 169, 104, 103, 119, 166, 40, 249, 56, 224,
        19, 148, 247, 105, 123, 163, 126, 134, 246, 201, 82, 149, 175, 151, 99, 213, 248, 157, 227, 130, 49,
        176, 146, 208, 45, 237, 140, 72, 11, 55, 109, 5, 83, 218, 160, 161, 2, 81, 28, 78, 135, 4, 127, 186,
        79, 155, 199, 219, 125, 228, 178, 46, 22, 61, 20, 193, 1, 182, 153, 118, 207, 102, 232, 93, 100,
        198, 50, 23, 204, 245, 25, 17, 197, 191, 254, 39, 173, 205, 32, 156, 33
};// This is not random for debug purposes
static const int gradientNbr = 4;
static const GLfloat gradients[gradientNbr * 3] = {
        1,1,0,      -1,1,0,
        1,-1,0,     -1,-1,0,
};

class PerlinTex {

    private:
        GLuint vertex_array_id_; // vertex array object
        GLuint program_id_; // GLSL shader program ID
        GLuint vertex_buffer_object_; // memory buffer
        GLuint texture_id_; // texture ID

    public:
        void Init() {
            // Perlin init


            // compile the shaders
            program_id_ = icg_helper::LoadShaders("perlinTex_vshader.glsl",
                                                  "perlinTex_fshader.glsl");

            if(!program_id_) {
                exit(EXIT_FAILURE);
            }

            glUseProgram(program_id_);

            // vertex one vertex Array
            glGenVertexArrays(1, &vertex_array_id_);
            glBindVertexArray(vertex_array_id_);

            // vertex coordinates
            {
                const GLfloat vertex_point[] = { /*V1*/ -1.0f, -1.0f, 0.0f,
                        /*V2*/ +1.0f, -1.0f, 0.0f,
                        /*V3*/ -1.0f, +1.0f, 0.0f,
                        /*V4*/ +1.0f, +1.0f, 0.0f};
                // buffer
                glGenBuffers(1, &vertex_buffer_object_);
                glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_point),
                             vertex_point, GL_STATIC_DRAW);

                // attribute
                GLint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
                glEnableVertexAttribArray(vertex_point_id);
                glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE,
                                      ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            }

            // texture coordinates
            {
                const GLfloat vertex_texture_coordinates[] = { /*V1*/ 0.0f, 0.0f,
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
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }


            // create 1D texture (gradient map)
            {
                glGenTextures(1, &texture_id_);
                glBindTexture(GL_TEXTURE_1D, texture_id_);
                glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, gradientNbr, 0, GL_RGB, GL_FLOAT, gradients);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                GLint tex_id = glGetUniformLocation(program_id_, "gradientMap");
                glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
                // check_error_gl();
            }

            // pass permutation as uniform variable
            glUniform1iv(glGetUniformLocation(program_id_, "P"), 256, permutaionArray);

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
        glDeleteTextures(1, &texture_id_);
    }

    void Draw() {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
