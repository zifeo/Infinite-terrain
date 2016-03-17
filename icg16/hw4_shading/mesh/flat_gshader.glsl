#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec4 vpoint_mv[3];
in vec3 light_dir_g[3], view_dir_g[3];

out vec3 normal_mv;
out vec3 light_dir, view_dir;

void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 4.2: Flat shading.
    // 1) compute the triangle normal normal_mv.
    vec3 orig = vpoint_mv[0].xyz;
    vec3 r = vpoint_mv[1].xyz - orig;
    vec3 l = vpoint_mv[2].xyz - orig;

    normal_mv = cross(r,l);

    //<<<<<<<<<< TODO <<<<<<<<<<<
    for(int i = 0; i < gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;
        light_dir = light_dir_g[i];
        view_dir = view_dir_g[i];
        EmitVertex();
    }
    EndPrimitive();
}
