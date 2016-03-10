#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    float distLeft = sqrt(uv.x * uv.x + (uv.y - 0.5) * (uv.y - 0.5));
    float heightLeft = sin(distLeft * 10 + time + 4) * 0.07;

    float distRightUpper = sqrt((uv.x - 1.0) * (uv.x - 1.0) + (uv.y - 1.0) * (uv.y - 1.0));
    float heightRightUpper = sin(distRightUpper * 12 + time - 4) * 0.07;

    float height = heightLeft + heightRightUpper;
    // float height = sin((uv.x + uv.y) * 10 + time) * 0.1; // Simple behavior
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}