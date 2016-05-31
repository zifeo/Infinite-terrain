#version 330

in vec2 uv;
in vec3 view_dir;
in vec3 light_dir;

out vec4 color;

uniform sampler2D normal_tex;
uniform sampler2D normal_tex2;
uniform sampler2D tex_reflect;

uniform float cam_posX;
uniform float cam_posY;

uniform float time;

// uniform sampler2D water;

void main() {

    ivec2 window_size = textureSize(tex_reflect, 0);
    vec2 uv2 = vec2(1 - gl_FragCoord.x / window_size.x, gl_FragCoord.y / window_size.y);

    float Ld = 0.15;
    float La = 0.85;
    float Ls = 0.5;

    vec2 texSize = textureSize(normal_tex, 0);
    vec2 new_uv = uv * 4;
    new_uv.x += cam_posX / 7.5 + 0.1 * sin(time / 5);
    new_uv.y += cam_posY / 7.5 + 0.1 * cos(time / 5);

    vec3 normal_mv =
        normalize(mix(texture(normal_tex, new_uv).xyz, texture(normal_tex2, new_uv).xyz, (1 + sin(time / 10) / 2)));
    normal_mv.x = (normal_mv.x * 2 - 1);
    normal_mv.y = -(normal_mv.y * 2 - 1);
    normal_mv.x *= 2;
    normal_mv.y *= 2;
    normal_mv = normalize(normal_mv);

    float nl = dot(normal_mv, light_dir);
    nl = nl >= 0 ? nl : 0;

    normal_mv.x *= 0.02;
    normal_mv.y *= 0.02;
    normal_mv = normalize(normal_mv);
    vec3 r_dir = reflect(-light_dir, normal_mv);

    float rv = dot(r_dir, view_dir);
    rv = rv >= 0 ? rv : 0;

    //;

    color = vec4(mix(texture(tex_reflect, uv2).rgb, vec3(0.2, 0.8, 1), 0.5) * (La + nl * Ld) +
                     vec3(1) * (pow(rv, 200) * Ls),
                 0.85);
    // color = vec4(vec3(pow(rv, 50)), 1);
}
