#pragma once

#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Water {

  private:
    GLuint vertex_array_id_;               // vertex array object
    GLuint vertex_buffer_object_position_; // memory buffer for positions
    GLuint vertex_buffer_object_index_;    // memory buffer for indices
    GLuint program_id_;                    // GLSL shader program ID
    GLuint normal_texture_id_;             // normal texture ID
    GLuint normal_texture2_id_;            // normal texture2 ID
    GLuint num_indices_;                   // number of vertices to render
    GLuint MVP_id_;                        // model, view, proj matrix ID
    GLuint MV_id_;
    GLuint time_id_;
    GLuint cam_pos_x_id_; // x value of the chunk
    GLuint cam_pos_y_id_; // y value of the chunk
    GLuint tex_reflection_id_;

  public:
    void Init(GLuint tex_reflection_id) {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("water_vshader.glsl", "water_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            vector<GLfloat> vertices;
            vector<GLuint> indices;
            int grid_dim = 256;

            float half = grid_dim / 2.0;

            for (int i = 0; i <= grid_dim; i++) {
                for (int j = 0; j <= grid_dim; j++) {
                    vertices.push_back((i - half) / half);
                    vertices.push_back((j - half) / half);
                    // cout << (i-half) / half << " " << (j-half) / half << " "
                    // <<
                    // (vertices.size()/2)-1 << "\n";
                }
            }

            for (int i = 0; i < grid_dim; i++) {
                for (int j = 0; j < grid_dim; j++) {
                    int ind = (grid_dim + 1) * i + j;
                    // cout << ind << "\n";
                    indices.push_back(0 + ind);
                    indices.push_back(1 + ind);
                    indices.push_back(grid_dim + 1 + ind);

                    indices.push_back(1 + ind);
                    indices.push_back(grid_dim + 1 + ind);
                    indices.push_back(grid_dim + 2 + ind);
                }
            }

            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        {
            initTexture("normal_texture_water.tga", &normal_texture_id_, "normal_tex", GL_TEXTURE0);
            initTexture("normal_texture_water2.tga", &normal_texture2_id_, "normal_tex2", GL_TEXTURE0 + 1);
        }

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        MV_id_ = glGetUniformLocation(program_id_, "MV");

        tex_reflection_id_ = tex_reflection_id;
        GLuint tex_reflect_id = glGetUniformLocation(program_id_, "tex_reflect");
        glUniform1i(tex_reflect_id, 2);

        cam_pos_x_id_ = glGetUniformLocation(program_id_, "cam_posX");
        cam_pos_y_id_ = glGetUniformLocation(program_id_, "cam_posY");
        glUniform1f(glGetUniformLocation(program_id_, "water_ampl"), WATER_AMPL);
        time_id_ = glGetUniformLocation(program_id_, "time");

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &normal_texture_id_);
        glDeleteTextures(1, &normal_texture2_id_);
        glDeleteTextures(1, &tex_reflection_id_);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX, const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX, glm::vec3 cam_pos = vec3(0, 0, 0)) {

        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_texture_id_);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, normal_texture2_id_);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, tex_reflection_id_);

        // setup MVP
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        glm::mat4 MV = view * model;
        glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        glUniform1f(cam_pos_x_id_, cam_pos.x);
        glUniform1f(cam_pos_y_id_, -cam_pos.z);

        // pass the current time stamp to the shader.
        glUniform1f(time_id_, time);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

  private:
    void initTexture(string filename, GLuint *texture_id, string texture_name, int val) {

        int width;
        int height;
        int nb_component;
        // set stb_image to have the same coordinates as OpenGL
        stbi_set_flip_vertically_on_load(1);
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

        if (image == nullptr) {
            throw(string("Failed to load texture"));
        }

        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if (nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        GLuint tex_id = glGetUniformLocation(program_id_, texture_name.c_str());
        glUseProgram(program_id_);
        glUniform1i(tex_id, val - GL_TEXTURE0);

        // cleanup
        glBindTexture(GL_TEXTURE_2D, val);
        stbi_image_free(image);
    }
};
