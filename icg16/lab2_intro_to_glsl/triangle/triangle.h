#pragma once
#include "icg_helper.h"


class Triangle {

    private:
        GLuint vertex_array_id_;        // vertex array object
        GLuint program_id_;             // GLSL shader program ID
        GLuint vertex_buffer_object_;   // memory buffer

    public:
        void Init() {
            // LoadShaders                  <--- compile the shaders
            // glUseProgram                 <--- bind the shader (for attributes)
            //---
            // glGenVertexArrays            <--- create vertex array
            // glBindVertexArray            <--- and bind it
            //---
            // glGenBuffers                 <--- generate buffer pointer
            // glBindBuffers                <--- make it current
            // glBufferData                 <--- tell it where to find data
            //---
            // glGetAttribLocation          <--- fetch attribute ID
            // glEnableVertexAttribArray    <--- make it the current
            // glVertexAttribPointer        <--- specify layout of attribute
            //---
            // stbi_load                    <--- load image from file
            // glGenTextures                <--- create texture buffer
            // glBindTexture                <--- and bind it
            // glTexImage2D                 <--- fill the texture with image
            // stbi_image_free              <--- free the image in cpu memory
            //---
            // glGetUniformLocation         <--- fetch ID from shader
            // glUniform*(...)              <--- set the values of a uniform
            //--- To avoid resource pollution, unload resources
            // glUseProgram(0)              <--- unbind program (safety!)
            // glBindVertexArray(0)         <--- unbind array (safety!)
        }

        void Cleanup() {
            // glDelete*(...);
        }
        
        void Draw() {
            // glUseProgram(_pid);
            // glBindVertexArray(_vao);
            //--- if there is a texture :
            // glActiveTexture          <--- enable certain tex-unit
            // glBindTexture            <--- bind it to CPU memory
            //--- if there a "changing" value to pass to the shader :
            // glUniform*(...)          <--- set the values of a uniform
            //---
            // glDraw*(...)             <--- draws the buffers
            //---
            // glBindVertexArray(0);
            // glUseProgram(0);
        }
};
