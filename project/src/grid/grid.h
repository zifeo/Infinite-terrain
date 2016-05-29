#pragma once

#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

struct Light {
    glm::vec3 La = glm::vec3(0.3f, 0.3f, 0.3f);
    glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 light_pos = glm::vec3(0, 2.0, 2.0);

    // pass light properties to the shader
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        // given in camera space
        glUniform3fv(glGetUniformLocation(program_id, "light_pos"), ONE, glm::value_ptr(light_pos));
        glUniform3fv(glGetUniformLocation(program_id, "La"), ONE, glm::value_ptr(La));
        glUniform3fv(glGetUniformLocation(program_id, "Ld"), ONE, glm::value_ptr(Ld));
        glUniform3fv(glGetUniformLocation(program_id, "Ls"), ONE, glm::value_ptr(Ls));
    }
};

struct Material {
    glm::vec3 ka = glm::vec3(0.18f, 0.1f, 0.1f);
    glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.5f);
    glm::vec3 ks = glm::vec3(0.0f, 0.0f, 0.0f);
    float alpha = 60.0f;

    // pass material properties to the shaders
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        glUniform3fv(glGetUniformLocation(program_id, "ka"), ONE, glm::value_ptr(ka));
        glUniform3fv(glGetUniformLocation(program_id, "kd"), ONE, glm::value_ptr(kd));
        glUniform3fv(glGetUniformLocation(program_id, "ks"), ONE, glm::value_ptr(ks));
        glUniform1f(glGetUniformLocation(program_id, "alpha"), alpha);
    }
};

class Grid : public Material, public Light {

  private:
    GLuint vertex_array_id_;               // vertex array object
    GLuint vertex_buffer_object_position_; // memory buffer for positions
    GLuint vertex_buffer_object_index_;    // memory buffer for indices
    GLuint program_id_;                    // GLSL shader program ID
    GLuint texture_id_;                    // texture ID
    GLuint sand_texture_id_;               // sand texture ID
    GLuint grass_texture_id_;              // grass texture ID
    GLuint rock_texture_id_;               // rock texture ID
    GLuint snow_texture_id_;               // snow texture ID
    GLuint num_indices_;                   // number of vertices to render
    GLuint MVP_id_;                        // model, view, proj matrix ID
    GLuint MV_id_;
    GLuint M_id_;
    GLuint P_id_;
    GLuint water_ampl_id_;
    GLuint x_chunk_id_;  // x value of the chunk
    GLuint y_chunk_id_;  // y value of the chunk
    GLuint clipping_id_; // clipping value

  public:
    void Init() {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("grid_vshader.glsl", "grid_fshader.glsl");
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
            int grid_dim = 64;

            float half = grid_dim / 2.f;
            int ind = 0;
            for (int i = 0; i < grid_dim; i++) {
                float posy0 = (i - half) / half;
                float posy1 = posy0 + 1 / half;

                if (i > 0) {
                    vertices.push_back(-1);
                    vertices.push_back(posy0);
                    indices.push_back(ind++);
                }

                for (int j = 0; j <= grid_dim; j++) {
                    float posx = (j - half) / half;

                    vertices.push_back(posx);
                    vertices.push_back(posy0);
                    indices.push_back(ind++);

                    vertices.push_back(posx);
                    vertices.push_back(posy1);
                    indices.push_back(ind++);
                }

                if (i < grid_dim) {
                    vertices.push_back(1);
                    vertices.push_back(posy1);
                    indices.push_back(ind++);
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
            initTexture("sand_texture.tga", &sand_texture_id_, "sand_tex", GL_TEXTURE0 + 1);
            initTexture("grass_texture.tga", &grass_texture_id_, "grass_tex", GL_TEXTURE0 + 2);
            initTexture("rock_texture.tga", &rock_texture_id_, "rock_tex", GL_TEXTURE0 + 3);
            initTexture("snow_texture.tga", &snow_texture_id_, "snow_tex", GL_TEXTURE0 + 4);
        }

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        MV_id_ = glGetUniformLocation(program_id_, "MV");
        M_id_ = glGetUniformLocation(program_id_, "M");
        P_id_ = glGetUniformLocation(program_id_, "P");
        water_ampl_id_ = glGetUniformLocation(program_id_, "water_ampl");

        x_chunk_id_ = glGetUniformLocation(program_id_, "x_chunk");
        y_chunk_id_ = glGetUniformLocation(program_id_, "y_chunk");
        clipping_id_ = glGetUniformLocation(program_id_, "clipping");

        glUniform3fv(glGetUniformLocation(program_id_, "light_pos"), ONE, glm::value_ptr(light_pos));
        glUniform3fv(glGetUniformLocation(program_id_, "La"), ONE, glm::value_ptr(La));
        glUniform3fv(glGetUniformLocation(program_id_, "Ld"), ONE, glm::value_ptr(Ld));
        glUniform3fv(glGetUniformLocation(program_id_, "Ls"), ONE, glm::value_ptr(Ls));

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
        glDeleteTextures(1, &texture_id_);
        glDeleteTextures(1, &sand_texture_id_);
        glDeleteTextures(1, &grass_texture_id_);
        glDeleteTextures(1, &rock_texture_id_);
        glDeleteTextures(1, &snow_texture_id_);
    }

    void Draw(GLuint texture_id, int x, int y, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX, const glm::mat4 &projection = IDENTITY_MATRIX,
              float clipping_height = 0.0f) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        Material::Setup(program_id_);
        Light::Setup(program_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, sand_texture_id_);

        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, grass_texture_id_);

        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, rock_texture_id_);

        glActiveTexture(GL_TEXTURE0 + 4);
        glBindTexture(GL_TEXTURE_2D, snow_texture_id_);

        // setup MVP
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        glm::mat4 MV = view * model;
        glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        // glm::mat4 M = model;
        // TODO : check
        glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        glm::mat4 P = projection;
        glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(P));

        glUniform1i(x_chunk_id_, x);
        glUniform1i(y_chunk_id_, y);
        glUniform1f(clipping_id_, clipping_height);

        glUniform1f(water_ampl_id_, WATER_AMPL * sin(time));

        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glCullFace(GL_FRONT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);
        // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

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
            throw string("Failed to load texture");
        }

        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);

        if (nb_component == 3) {
            glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGB, width, height);
            // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, image);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nb_component == 4) {
            glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA, width, height);
            // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, image);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        glGenerateMipmap(GL_TEXTURE_2D); // Generate num_mipmaps number of mipmaps here.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        GLuint tex_id = glGetUniformLocation(program_id_, texture_name.c_str());
        glUseProgram(program_id_);
        glUniform1i(tex_id, val - GL_TEXTURE0);

        // cleanup
        glBindTexture(GL_TEXTURE_2D, val);
        stbi_image_free(image);
    }
};
