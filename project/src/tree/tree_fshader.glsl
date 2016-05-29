#version 330

#define TEXSIZE 1024

in vec2 uv;
in float dist;

out vec4 color;

uniform sampler2D tree_tex;

void main() {

    color = vec4(texture(tree_tex, uv).rgba);
    vec4 brouillard = vec4(0.8, 0.8, 0.8, color.a);
    color = mix(color, brouillard, clamp(dist * dist / (9 * 9), 0, 1));
    if (color.a < 0.5) {
        discard;
    }
}
