#pragma once
#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

#define NB_COLOR_1D 100

GLfloat tex[NB_COLOR_1D * 3] = {
    0.384314, 0.666667, 0.243137, 0.392157, 0.65098,  0.258824, 0.396078, 0.639216, 0.270588,
    0.396078, 0.623529, 0.282353, 0.403922, 0.607843, 0.298039, 0.403922, 0.603922, 0.301961,
    0.403922, 0.596078, 0.305882, 0.403922, 0.592157, 0.309804, 0.407843, 0.588235, 0.313725,
    0.407843, 0.588235, 0.313725, 0.407843, 0.584314, 0.317647, 0.407843, 0.584314, 0.317647,
    0.407843, 0.580392, 0.317647, 0.407843, 0.584314, 0.317647, 0.407843, 0.584314, 0.317647,
    0.407843, 0.584314, 0.317647, 0.407843, 0.584314, 0.317647, 0.407843, 0.588235, 0.313725,
    0.407843, 0.592157, 0.313725, 0.407843, 0.592157, 0.309804, 0.403922, 0.596078, 0.309804,
    0.403922, 0.6,      0.305882, 0.403922, 0.603922, 0.301961, 0.403922, 0.603922, 0.301961,
    0.4,      0.607843, 0.298039, 0.4,      0.611765, 0.290196, 0.396078, 0.615686, 0.286275,
    0.392157, 0.611765, 0.27451,  0.384314, 0.615686, 0.270588, 0.376471, 0.615686, 0.258824,
    0.368627, 0.615686, 0.25098,  0.360784, 0.611765, 0.239216, 0.356863, 0.611765, 0.231373,
    0.352941, 0.603922, 0.227451, 0.345098, 0.607843, 0.219608, 0.341176, 0.607843, 0.215686,
    0.341176, 0.603922, 0.215686, 0.341176, 0.607843, 0.215686, 0.345098, 0.607843, 0.219608,
    0.352941, 0.607843, 0.227451, 0.364706, 0.611765, 0.239216, 0.368627, 0.607843, 0.25098,
    0.384314, 0.603922, 0.278431, 0.396078, 0.592157, 0.294118, 0.415686, 0.580392, 0.309804,
    0.427451, 0.560784, 0.329412, 0.427451, 0.545098, 0.341176, 0.435294, 0.533333, 0.352941,
    0.435294, 0.52549,  0.360784, 0.431373, 0.521569, 0.360784, 0.435294, 0.521569, 0.364706,
    0.435294, 0.521569, 0.364706, 0.439216, 0.537255, 0.368627, 0.443137, 0.545098, 0.372549,
    0.454902, 0.564706, 0.376471, 0.462745, 0.584314, 0.392157, 0.482353, 0.596078, 0.419608,
    0.505882, 0.623529, 0.443137, 0.541176, 0.65098,  0.482353, 0.564706, 0.666667, 0.517647,
    0.592157, 0.67451,  0.552941, 0.627451, 0.694118, 0.588235, 0.654902, 0.709804, 0.631373,
    0.670588, 0.717647, 0.647059, 0.701961, 0.733333, 0.686275, 0.721569, 0.745098, 0.709804,
    0.741176, 0.756863, 0.733333, 0.752941, 0.764706, 0.74902,  0.768627, 0.776471, 0.764706,
    0.784314, 0.788235, 0.784314, 0.8,      0.803922, 0.8,      0.811765, 0.811765, 0.811765,
    0.827451, 0.827451, 0.827451, 0.839216, 0.839216, 0.839216, 0.85098,  0.85098,  0.85098,
    0.862745, 0.862745, 0.862745, 0.870588, 0.870588, 0.870588, 0.882353, 0.882353, 0.882353,
    0.894118, 0.894118, 0.894118, 0.901961, 0.901961, 0.901961, 0.913725, 0.913725, 0.913725,
    0.921569, 0.921569, 0.921569, 0.92549,  0.92549,  0.92549,  0.929412, 0.929412, 0.929412,
    0.933333, 0.933333, 0.933333, 0.937255, 0.937255, 0.937255, 0.941176, 0.941176, 0.941176,
    0.945098, 0.945098, 0.945098, 0.94902,  0.94902,  0.94902,  0.952941, 0.952941, 0.952941,
    0.956863, 0.956863, 0.956863, 0.960784, 0.960784, 0.960784, 0.964706, 0.964706, 0.964706,
    0.968627, 0.968627, 0.968627, 0.976471, 0.976471, 0.976471, 0.980392, 0.980392, 0.980392,
    0.984314, 0.984314, 0.984314, 0.988235, 0.988235, 0.988235, 0.992157, 0.992157, 0.992157,
    0.996078, 0.996078, 0.996078};

struct Light {
    glm::vec3 La = glm::vec3(0.3f, 0.3f, 0.3f);
    glm::vec3 Ld = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 light_pos = glm::vec3(0, 2.0, 2.0);

    // pass light properties to the shader
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        // given in camera space
        GLuint light_pos_id = glGetUniformLocation(program_id, "light_pos");

        GLuint La_id = glGetUniformLocation(program_id, "La");
        GLuint Ld_id = glGetUniformLocation(program_id, "Ld");
        GLuint Ls_id = glGetUniformLocation(program_id, "Ls");

        glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
        glUniform3fv(La_id, ONE, glm::value_ptr(La));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
        glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));
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

        GLuint ka_id = glGetUniformLocation(program_id, "ka");
        GLuint kd_id = glGetUniformLocation(program_id, "kd");
        GLuint ks_id = glGetUniformLocation(program_id, "ks");
        GLuint alpha_id = glGetUniformLocation(program_id, "alpha");

        glUniform3fv(ka_id, ONE, glm::value_ptr(ka));
        glUniform3fv(kd_id, ONE, glm::value_ptr(kd));
        glUniform3fv(ks_id, ONE, glm::value_ptr(ks));
        glUniform1f(alpha_id, alpha);
    }
};

class Grid : public Material, public Light {

  private:
    GLuint vertex_array_id_;               // vertex array object
    GLuint vertex_buffer_object_position_; // memory buffer for positions
    GLuint vertex_buffer_object_index_;    // memory buffer for indices
    GLuint program_id_;                    // GLSL shader program ID
    GLuint texture_1d_id_;                 // texture 1D ID
    GLuint texture_id_;                    // texture ID
    GLuint num_indices_;                   // number of vertices to render
    GLuint MVP_id_;                        // model, view, proj matrix ID
    GLuint MV_id_;
    GLuint M_id_;
    GLuint P_id_;

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
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;
            // TODO 5: make a triangle grid with dimension 100x100.
            // always two subsequent entries in 'vertices' form a 2D vertex
            // position.
            int grid_dim = 512;

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
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0],
                         GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],
                         GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE,
                                  ZERO_BUFFER_OFFSET);
        }

        {
            GLuint tex_normal_id = glGetUniformLocation(program_id_, "tex");
            glUniform1i(tex_normal_id, 0 /*GL_TEXTURE0*/);

            // 1D texture
            glGenTextures(1, &texture_1d_id_);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_1D, texture_1d_id_);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, NB_COLOR_1D, 0, GL_RGB, GL_FLOAT, tex);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLuint tex_1d_id = glGetUniformLocation(program_id_, "tex1D");
            glUniform1i(tex_1d_id, 1 /*GL_TEXTURE0*/);

            glBindTexture(GL_TEXTURE_1D, 0);
        }

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");
        MV_id_ = glGetUniformLocation(program_id_, "MV");
        M_id_ = glGetUniformLocation(program_id_, "M");
        P_id_ = glGetUniformLocation(program_id_, "P");

        GLuint light_pos_id = glGetUniformLocation(program_id_, "light_pos");

        GLuint La_id = glGetUniformLocation(program_id_, "La");
        GLuint Ld_id = glGetUniformLocation(program_id_, "Ld");
        GLuint Ls_id = glGetUniformLocation(program_id_, "Ls");

        glUniform3fv(light_pos_id, ONE, glm::value_ptr(light_pos));
        glUniform3fv(La_id, ONE, glm::value_ptr(La));
        glUniform3fv(Ld_id, ONE, glm::value_ptr(Ld));
        glUniform3fv(Ls_id, ONE, glm::value_ptr(Ls));

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
    }

    void Draw(GLint texture_id, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        Material::Setup(program_id_);
        Light::Setup(program_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        // bind textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, texture_1d_id_);

        // setup MVP
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        glm::mat4 MV = view * model;
        glUniformMatrix4fv(MV_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        //glm::mat4 M = model;
        // TODO : check
        glUniformMatrix4fv(M_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MV));

        glm::mat4 P = projection;
        glUniformMatrix4fv(P_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(P));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};
