#version 330

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;

float PI = 3.141596;

void main() {
    color = texture(colormap, ((sin(uv.x*10*PI) * sin(uv.y*10*PI)) + 1)/2).rgb;
}




