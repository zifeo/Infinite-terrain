#pragma once
#include "icg_helper.h"
#include "glm/gtc/type_ptr.hpp"


// Perlin utilities
static const GLint permutaionArray[256] = {
        177, 139, 228, 183, 157, 36, 135, 142, 75, 245, 40, 39, 195, 133, 171, 182, 78, 210, 174, 170, 151, 18, 165, 216, 112, 141, 20, 49, 15, 81, 5, 167, 79, 70, 119, 158, 83, 193, 178, 96, 137, 7, 169, 61, 247, 91, 207, 159, 114, 74, 90, 41, 144, 212, 243, 176, 163, 80, 194, 55, 109, 37, 22, 229, 28, 87, 84, 98, 199, 117, 225, 3, 77, 111, 58, 236, 63, 241, 254, 6, 125, 129, 252, 180, 120, 110, 60, 64, 92, 233, 155, 200, 203, 128, 103, 88, 214, 191, 51, 46, 175, 238, 220, 115, 160, 45, 221, 251, 31, 95, 161, 249, 222, 123, 248, 138, 242, 26, 59, 122, 179, 42, 244, 29, 72, 23, 153, 204, 113, 156, 149, 143, 35, 9, 235, 89, 166, 62, 1, 13, 24, 14, 66, 106, 99, 189, 136, 206, 33, 198, 67, 69, 25, 131, 94, 52, 47, 102, 30, 232, 12, 34, 188, 162, 104, 147, 71, 56, 227, 21, 76, 0, 185, 48, 197, 173, 8, 148, 27, 213, 211, 11, 164, 255, 201, 65, 234, 132, 43, 219, 218, 32, 184, 140, 126, 145, 224, 16, 253, 172, 101, 230, 226, 85, 73, 100, 93, 17, 124, 107, 150, 146, 187, 192, 38, 108, 240, 130, 118, 209, 250, 19, 196, 217, 127, 186, 237, 190, 215, 57, 239, 154, 105, 223, 152, 50, 116, 53, 2, 205, 44, 231, 168, 202, 4, 134, 68, 121, 86, 208, 97, 10, 82, 54, 246, 181
};// This is not random for debug purposes
static const int gradientNbr = 5;
static const GLfloat gradients[gradientNbr * 3] = {
        1,1,0,      -1,1,0,
        -1,-1,0,     1,-1,0,
        1,1,0,
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
            glUniform1iv(glGetUniformLocation(program_id_, "Perm"), 256, permutaionArray);

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

    void Draw(int octave, float lac, float H) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        glUniform1i(glGetUniformLocation(program_id_, "octave"), octave);
        glUniform1f(glGetUniformLocation(program_id_, "lac"), lac);
        glUniform1f(glGetUniformLocation(program_id_, "H"), H);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
