#version 330

#define SKIRT 0.001

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform mat4 MV;
uniform float time;
uniform float angle;

uniform int x_chunk;
uniform int y_chunk;

uniform float x_in_chunk;
uniform float y_in_chunk;

uniform sampler2D perlin_tex;

void main() {
    uv = (position + vec2(1.0)) * 8;
    uv.x += 0.5;
    uv.y += 0.5;

    vec3 pos_3d = vec3(position.x * cos(angle), texture(perlin_tex, vec2(x_in_chunk / 2 + 0.5, y_in_chunk / 2 + 0.5)).x * 2 - 1 + position.y + 1/16.0f, position.x * sin(angle));
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
