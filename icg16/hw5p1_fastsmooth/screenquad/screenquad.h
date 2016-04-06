#pragma once
#include "icg_helper.h"

struct PASS {
        enum ENUM {
            FIRST=1, SECOND=2
        };
};

class ScreenQuad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint fast_program_id_;        // GLSL fast shader program ID
        GLuint slow_program_id_;        // GLSL slow shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_1_id_;           // texture ID 1
        GLuint texture_2_id_;           // texture ID 2

        float std_dev = 2;

        int screenquad_width_;
        int screenquad_height_;

    public:
        void Init(int width, int height, GLuint texture, GLuint texture2) {

            // set screenquad size
            this->screenquad_width_ = width;
            this->screenquad_height_ = height;

            // compile the shaders
            fast_program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_fshader.glsl");
            if(!fast_program_id_) {
                exit(EXIT_FAILURE);
            }

            slow_program_id_ = icg_helper::LoadShaders("screenquad_vshader.glsl",
                                                  "screenquad_slow_fshader.glsl");
            if(!slow_program_id_) {
                exit(EXIT_FAILURE);
            }

            program_id_ = slow_program_id_;

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
                GLuint vertex_point_id = glGetAttribLocation(program_id_, "vpoint");
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
                GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_,
                                                                     "vtexcoord");
                glEnableVertexAttribArray(vertex_texture_coord_id);
                glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT,
                                      DONT_NORMALIZE, ZERO_STRIDE,
                                      ZERO_BUFFER_OFFSET);
            }

            // load/Assign texture
            this->texture_1_id_ = texture;
            glBindTexture(GL_TEXTURE_2D, texture_1_id_);
            glUniform1i(glGetUniformLocation(program_id_, "tex_PASS1"),
                        0 /*GL_TEXTURE0*/);

            // load/Assign texture2
            this->texture_2_id_ = texture2;
            glBindTexture(GL_TEXTURE_2D, texture_2_id_);
            glUniform1i(glGetUniformLocation(program_id_, "tex_PASS2"),
                        1 /*GL_TEXTURE1*/);

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
            glDeleteTextures(1, &texture_1_id_);
            glDeleteTextures(1, &texture_2_id_);
        }

        void toggleBlurSpeed(bool fast) {
            program_id_ = fast ? fast_program_id_ : slow_program_id_;
            std::cout << "fast mode : " << (fast ? "on" : "off") << std::endl;
        }

        void UpdateSize(int screenquad_width, int screenquad_height) {
            this->screenquad_width_ = screenquad_width;
            this->screenquad_height_ = screenquad_height;
        }

        void UpdateStandardDeviation(float multiplier){
            std_dev *= multiplier;
            std::cout << "new std_dev : " << std_dev << std::endl;
        }
        
        void Draw(PASS::ENUM pass_nb) {
            glUseProgram(program_id_);
            glBindVertexArray(vertex_array_id_);

            glUniform1f(glGetUniformLocation(program_id_, "std"), std_dev);
            glUniform1i(glGetUniformLocation(program_id_, "pass_nb"), pass_nb);
            glUniform1f(glGetUniformLocation(program_id_, "tex_width"),
                        screenquad_width_);
            glUniform1f(glGetUniformLocation(program_id_, "tex_height"),
                        screenquad_height_);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_1_id_);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_2_id_);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindVertexArray(0);
            glUseProgram(0);
        }
};
