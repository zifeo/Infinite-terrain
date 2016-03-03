#version 330

const vec3 COLORS[] = vec3[3](
    vec3(1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, 0, 1)
);

uniform mat4 M;
in vec3 vpoint;
out vec3 fcolor;

void main() {
    gl_Position = M * vec4(vpoint, 1.0);
    fcolor = COLORS[gl_VertexID];
}
