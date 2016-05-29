#version 330

#define SKIRT 0.001

in vec2 position;

out vec2 uv;
out vec3 light_dir;
out float height;
out float dist;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 light_pos;

uniform float clipping;

uniform int x_chunk;
uniform int y_chunk;

uniform sampler2D tex;

void main() {

    uv = (position + vec2(1.0)) * 0.5;
    height = texture(tex, uv).x;

    vec3 pos_3d = vec3(position.x, height * 2 - 1, -position.y);

    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);

    gl_Position = MVP * vec4(pos_3d, 1.0);

    dist = length(gl_Position.xyz);

    gl_ClipDistance[0] = height - clipping;

    light_dir = normalize(light_pos.xyz - vpoint_mv.xyz);
    height = clamp(height, 0, 1);
}
