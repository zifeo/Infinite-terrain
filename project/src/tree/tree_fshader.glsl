#version 330

#define TEXSIZE 1024

in vec2 uv;
in float discardVertex;

out vec4 color;

uniform sampler2D perlin_tex;
uniform sampler2D tree_tex;

void main() {
    if (discardVertex == 1.0f) {
        discard;
    }

    color = vec4(texture(tree_tex, uv).rgba);
    if (color.a < 0.5) {
        discard;
    }
}
