#pragma once

#include <GL/glew.h>
#include "icg_helper.h"

class Quad {

private:
    GLuint vertex_array_id_;        // vertex array object
    GLuint program_id_;             // GLSL shader program ID
    GLuint vertex_buffer_object_;   // memory buffer
    GLuint texture_id_;             // texture ID

    GLfloat vpoint[16] = {
            /*V1*/ -1.0f, -1.0f, 0.0f,
            /*V2*/ +1.0f, -1.0f, 0.0f,
            /*V3*/ -1.0f, +1.0f, 0.0f,
            /*V4*/ +1.0f, +1.0f, 0.0f
    };

public:
    void Init() {
        program_id_ = icg_helper::LoadShaders("quad_vshader.glsl",
                                              "quad_fshader.glsl");
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

        const GLfloat vertex_texture_coordinates[] = {
                /*V1*/ 0.0f, 0.0f,
                /*V2*/ 1.0f, 0.0f,
                /*V3*/ 0.0f, 1.0f,
                /*V4*/ 1.0f, 1.0f
        };

        // buffer
        glGenBuffers(1, &vertex_buffer_object_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_texture_coordinates),
                     vertex_texture_coordinates, GL_STATIC_DRAW);
        // attribute
        GLuint vertex_texture_coord_id = glGetAttribLocation(program_id_, "vtexcoord");
        glEnableVertexAttribArray(vertex_texture_coord_id);
        glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        int width, height, nb_component;
        string filename = "mrt.tga";
        stbi_set_flip_vertically_on_load(1);
        unsigned char* image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);
        if(image == nullptr) {
            throw(string("Failed to load texture"));
        }

        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if(nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if(nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, image);
        }
        GLuint tex_id = glGetUniformLocation(program_id_, "tex");
        glUniform1i(tex_id, 0 /*GL_TEXTURE0*/);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
    }

    void Cleanup() {
        //...
    }

    void Draw() {
        //...
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
};
