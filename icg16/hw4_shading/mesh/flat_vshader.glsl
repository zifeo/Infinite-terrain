#version 330

in vec3 vpoint;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

void main() {
    mat4 MV = view * model;
    vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 4.1: Flat shading.
    // 2) compute the light direction light_dir.
    light_dir = normalize(light_pos - vpoint_mv.xyz);
    // 3) compute the view direction view_dir.
    view_dir = normalize(-vpoint_mv.xyz);
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
