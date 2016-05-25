#version 330

#define SKIRT 0.001

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform mat4 MV;
uniform float time;

uniform int x_chunk;
uniform int y_chunk;

uniform sampler2D tex;

void main() {
    uv = (position + vec2(1.0)) * 0.5;
    float x = uv.x + x_chunk;
    float y = uv.y + y_chunk;

    float height =
        -0.1 + 0.006 * sin(40 * sin(x * y) + time) +
        0.006 * sin(40 * cos(y) + time); // + 0.006 * sin(40 * x * y + time) + 0.006 * sin(40 * (-x) * y + time)

    vec3 pos_3d = vec3(position.x, -0.2, -position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
