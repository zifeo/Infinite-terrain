#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

#define M_PI 3.1415926535897932384626433832795

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    float distLeft = sqrt((uv.x + 0.5) * (uv.x + 0.5) + (uv.y - 0.5) * (uv.y - 0.5));
    float heightLeft = pow((sin(distLeft * 5 * M_PI - 2*time) + 1)/2, 2) * 0.14;

    float distRightUpper = sqrt((uv.x - 1.0) * (uv.x - 1.0) + (uv.y - 1.0) * (uv.y - 1.0));
    float heightRightUpper = pow((sin(distRightUpper * 4 * M_PI - time) + 1)/2, 2) * 0.12;

    float height = heightLeft + heightRightUpper - 0.26;
    // float height = sin((uv.x + uv.y) * 10 + time) * 0.1; // Simple behavior
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
