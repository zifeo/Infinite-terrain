#pragma once
#include "icg_helper.h"

class Quad {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer
        GLuint texture_id_;             // texture ID

    public:
        void Init() {
            //...
        }

        void Cleanup() {
            //...
        }

        void Draw() {
            //...
        }
};
