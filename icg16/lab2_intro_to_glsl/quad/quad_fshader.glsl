#version 330

const vec3 COLORS[2] = vec3[](
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0)
);

uniform sampler2D tex;
in vec2 uv;
out vec3 color;

void main() {
    color = texture(tex,uv).rgb;
}
