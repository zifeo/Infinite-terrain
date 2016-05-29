#version 330 core

in vec3 vpoint;
in vec2 vtexcoord;
out vec2 point;

uniform int X;
uniform int Y;

mat4 S(float s) { return mat4(mat3(s)); }

void main() {
    gl_Position = S(1) * vec4(vpoint, 1.0);
    point = (vpoint.xy) * 513. / 512. - 1. / 513 + vec2(X, Y) * 2;
}
