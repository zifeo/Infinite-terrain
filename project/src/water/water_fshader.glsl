#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D water_tex;
uniform sampler2D tex_reflect;

// uniform sampler2D water;

void main() {


    ivec2 window_size = textureSize(tex_reflect, 0);
    vec2 uv2 = vec2(gl_FragCoord.x/window_size.x, gl_FragCoord.y/window_size.y);

    color = vec4(mix(texture(tex_reflect, uv).rgb, vec3(1, 0, 1), 0.25), 1);

    //color = vec4(texture(tex_reflect, uv).rgb, 1);
    // color = vec4(1, 0, 1, 1);
    // color = vec4(texture(water_tex, uv).rgb, 0.6);
}
