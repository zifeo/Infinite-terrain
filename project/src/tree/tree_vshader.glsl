#version 330

#define SKIRT 0.001

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform mat4 MV;
uniform float time;
uniform float angle;

uniform float tree_height;

void main() {
    uv = (position + vec2(1.0)) * (1 / tree_height) / 2;
    uv.x += 0.5;
    uv.y += 0.5;

    vec3 pos_3d = vec3(position.x * cos(angle), position.y + tree_height, position.x * sin(angle));
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
