#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "icg_helper.h"

// Perlin utilities
static const GLint permutaionArray[256] = {
    254, 183, 91,  193, 118, 36,  210, 124, 154, 94,  4,   51,  218, 157, 55,  155, 81,  241, 251, 172, 9,   44,
    71,  188, 132, 108, 106, 63,  148, 96,  120, 131, 57,  237, 252, 6,   162, 191, 90,  250, 66,  228, 213, 86,
    29,  230, 75,  198, 128, 236, 196, 3,   159, 211, 166, 206, 197, 160, 23,  142, 207, 169, 58,  113, 7,   127,
    139, 249, 225, 30,  38,  114, 121, 226, 182, 240, 202, 79,  187, 83,  33,  42,  27,  68,  65,  61,  73,  109,
    122, 47,  158, 2,   153, 17,  246, 14,  229, 20,  69,  136, 239, 126, 12,  92,  98,  112, 152, 87,  244, 245,
    46,  141, 135, 222, 234, 22,  171, 200, 99,  190, 1,   147, 24,  224, 123, 95,  189, 195, 110, 186, 133, 50,
    31,  103, 35,  13,  15,  177, 107, 205, 199, 77,  48,  54,  180, 181, 16,  28,  37,  102, 72,  174, 89,  115,
    129, 176, 41,  219, 217, 105, 238, 179, 145, 204, 60,  52,  64,  173, 194, 119, 130, 151, 111, 76,  143, 5,
    101, 223, 178, 168, 80,  67,  255, 59,  208, 21,  53,  18,  34,  97,  78,  11,  88,  233, 117, 209, 220, 247,
    149, 203, 100, 74,  40,  146, 214, 161, 93,  175, 8,   137, 82,  216, 116, 43,  45,  25,  192, 185, 163, 10,
    62,  39,  104, 201, 156, 134, 85,  140, 227, 232, 243, 212, 221, 184, 125, 144, 19,  167, 253, 49,  242, 56,
    150, 84,  231, 70,  215, 235, 165, 256, 26,  164, 170, 138, 32,  248}; // This is not random for debug purposes

static const int gradientNbr = 5;
static const GLfloat gradients[gradientNbr * 3] = {
    1, 1, 0, -1, 1, 0, -1, -1, 0, 1, -1, 0, 1, 1, 0,
};

class PerlinTex {

  private:
    GLuint vertex_array_id_;      // vertex array object
    GLuint program_id_;           // GLSL shader program ID
    GLuint vertex_buffer_object_; // memory buffer
    GLuint texture_id_;           // texture ID

  public:
    void Init() {
        // compile the shaders
        program_id_ = icg_helper::LoadShaders("perlinTex_vshader.glsl", "perlinTex_fshader.glsl");
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
            GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
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
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates), vertex_texture_coordinates,
                         GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // create 1D texture (gradient map)
        {

            glGenTextures(1, &texture_id_);
            glBindTexture(GL_TEXTURE_1D, texture_id_);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, gradientNbr, 0, GL_RGB, GL_FLOAT, gradients);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            GLuint tex_id = glGetUniformLocation(program_id_, "gradientMap");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

            glBindTexture(GL_TEXTURE_1D, 0);
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

    void Draw(int octave, float lac, float H, int X, int Y) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // cout << "X : " << X << ", Y : " << Y << endl;

        glUniform1i(glGetUniformLocation(program_id_, "octaves"), octave);
        glUniform1f(glGetUniformLocation(program_id_, "lac"), lac);
        glUniform1f(glGetUniformLocation(program_id_, "H"), H);
        glUniform1i(glGetUniformLocation(program_id_, "X"), X);
        glUniform1i(glGetUniformLocation(program_id_, "Y"), Y);

        glUniform1i(glGetUniformLocation(program_id_, "temperature_octave"), TEMPERATURE_OCTAVE);
        glUniform1f(glGetUniformLocation(program_id_, "temperature_lac"), TEMPERATURE_LAC);
        glUniform1f(glGetUniformLocation(program_id_, "temperature_H"), TEMPERATURE_H);

        glUniform1i(glGetUniformLocation(program_id_, "altitude_octave"), ALTITUDE_OCTAVE);
        glUniform1f(glGetUniformLocation(program_id_, "altitude_lac"), ALTITUDE_LAC);
        glUniform1f(glGetUniformLocation(program_id_, "altitude_H"), ALTITUDE_H);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, texture_id_);

        // draw
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
