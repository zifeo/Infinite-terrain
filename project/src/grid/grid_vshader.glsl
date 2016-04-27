#version 330

#define SKIRT 0.001

in vec2 position;
in vec2 vtexcoord;

out vec2 uv;
out vec3 light_dir;
out float height;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 light_pos;
uniform float time;

uniform sampler2D tex;

void main() {
    uv = (position + vec2(1.0)) * 0.5;
    height = texture(tex, uv).x;

    vec3 pos_3d = vec3(position.x, height*2 - 1, -position.y);

    if (position.x < -1.0f + SKIRT || position.x > 1.0f - SKIRT || position.y < -1.0f + SKIRT || position.y > 1.0f - SKIRT)
        pos_3d = vec3(pos_3d.x, pos_3d.y-0.2, pos_3d.z);

    vec4 vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = MVP * vec4(pos_3d, 1.0);

    light_dir = normalize(light_pos.xyz - vpoint_mv.xyz);
}
