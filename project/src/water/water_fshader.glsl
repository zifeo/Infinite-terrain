#version 330

#define TEXSIZE 1024

in vec2 uv;

out vec4 color;

uniform sampler2D water_tex;

// uniform sampler2D water;

void main() { color = vec4(texture(water_tex, uv).rgb, 0.6); }
