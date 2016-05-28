#version 330

#define SKIRT 0.001

in vec2 position;

out vec2 uv;
out vec3 view_dir;
out vec3 light_dir;

uniform mat4 MVP;
uniform mat4 MV;
uniform float time;

void main() {
    uv = (position+1)/6;

    vec3 pos_3d = vec3(position.x, 0 + 0.1*sin(time), -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);

    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
    view_dir = normalize(-vpoint_mv.xyz);
    vec4 light_pos = vec4(0, 1, 0, 1);
    light_dir = normalize(light_pos.xyz-vpoint_mv.xyz);
}
