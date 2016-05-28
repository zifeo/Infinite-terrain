#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "icg_helper.h"

using namespace std;
using namespace glm;

static const unsigned int NbCubeVertices = 36;

static const vec3 CubeVertices[] = {
    vec3(-0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5),  vec3(0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),  vec3(0.5, 0.5, -0.5),   vec3(0.5, 0.5, 0.5),   vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),   vec3(0.5, -0.5, 0.5),   vec3(0.5, 0.5, -0.5),  vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, 0.5),    vec3(-0.5, 0.5, 0.5),   vec3(0.5, -0.5, 0.5),  vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),   vec3(-0.5, -0.5, 0.5),  vec3(-0.5, -0.5, 0.5), vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),   vec3(-0.5, -0.5, -0.5), vec3(0.5, -0.5, 0.5),  vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),  vec3(-0.5, -0.5, -0.5), vec3(-0.5, 0.5, 0.5),  vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),   vec3(-0.5, -0.5, 0.5),  vec3(0.5, 0.5, -0.5),  vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),    vec3(-0.5, 0.5, -0.5),  vec3(0.5, 0.5, 0.5),   vec3(-0.5, 0.5, 0.5)};
// TODO which best

static const unsigned int NbCubeUVs = 36;

static const vec2 CubeUVs[] = {
    vec2(0.333, 0.75), vec2(0.666, 0.75), vec2(0.333, 0.5),  vec2(0.666, 0.75), vec2(0.333, 0.5),  vec2(0.666, 0.5),
    vec2(0.666, 0.25), vec2(0.333, 0.25), vec2(0.666, 0.5),  vec2(0.333, 0.25), vec2(0.666, 0.5),  vec2(0.333, 0.5),
    vec2(0.666, 0.25), vec2(0.666, 0.0),  vec2(0.333, 0.25), vec2(0.666, 0.0),  vec2(0.333, 0.25), vec2(0.333, 0.0),
    vec2(0.0, 0.75),   vec2(0.333, 0.75), vec2(0.0, 0.5),    vec2(0.333, 0.75), vec2(0.0, 0.5),    vec2(0.333, 0.5),
    vec2(0.666, 0.75), vec2(0.333, 0.75), vec2(0.666, 1.0),  vec2(0.333, 0.75), vec2(0.666, 1.0),  vec2(0.333, 1.0),
    vec2(0.666, 0.5),  vec2(0.666, 0.75), vec2(1.0, 0.5),    vec2(0.666, 0.75), vec2(1.0, 0.5),    vec2(1.0, 0.75)};

class Sky {

  private:
    GLuint program_id_;           // GLSL shader program ID
    GLuint vertex_array_id_;      // vertex array object
    GLuint vertex_buffer_object_; // memory buffer
    GLuint texture_id_;           // texture ID
    mat4 model_matrix_;           // model matrix
    GLuint mvp_id_;

  public:
    void Init() {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("sky_vshader.glsl", "sky_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates
        {
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, NbCubeVertices * sizeof(vec3), &CubeVertices[0], GL_STATIC_DRAW);

            // attribute
            GLuint vertex_point_id = (GLuint)glGetAttribLocation(program_id_, "vpoint");
            glEnableVertexAttribArray(vertex_point_id);
            glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, (GLboolean)DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // texture coordinates
        {
            // buffer
            glGenBuffers(1, &vertex_buffer_object_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
            glBufferData(GL_ARRAY_BUFFER, NbCubeUVs * sizeof(vec2), &CubeUVs[0], GL_STATIC_DRAW);

            // attribute
            GLuint vertex_texture_coord_id = (GLuint)glGetAttribLocation(program_id_, "vtexcoord");
            glEnableVertexAttribArray(vertex_texture_coord_id);
            glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, (GLboolean)DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        // load texture
        {
            int width;
            int height;
            int nb_component;
            string texture_filename = "sky_texture.tga";
            stbi_set_flip_vertically_on_load(1);
            unsigned char *image = stbi_load(texture_filename.c_str(), &width, &height, &nb_component, 0);

            if (image == nullptr) {
                throw string("Failed to load texture");
            }

            glGenTextures(1, &texture_id_);
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            if (nb_component == 3) {

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

            } else if (nb_component == 4) {

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }

            GLuint tex_id = (GLuint)glGetUniformLocation(program_id_, "tex");
            glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);

            // cleanup
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(image);
        }

        mvp_id_ = glGetUniformLocation(program_id_, "MVP");

        model_matrix_ = scale(model_matrix_, vec3(100.0f));
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_);
        glDeleteProgram(program_id_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteTextures(1, &texture_id_);
    }

    void Draw(const mat4 &view_projection) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        // glUniform1f(glGetUniformLocation(program_id_, "time"), glfwGetTime());

        // setup MVP
        mat4 MVP = view_projection * model_matrix_;
        glUniformMatrix4fv(mvp_id_, 1, GL_FALSE, value_ptr(MVP));

        // draw
        glDrawArrays(GL_TRIANGLES, 0, NbCubeVertices);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
