#version 330

#define TEXSIZE 1024

in vec2 uv;
in vec3 colorHeight;
in vec3 light_dir;

out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex;

const ivec2 l = ivec2(-1,0);
const ivec2 r = ivec2(1,0);

void main() {
    float perlinValue = texture(tex, uv).x;

    float dxl = textureOffset(tex, uv, l).x;
    float dxr = textureOffset(tex, uv, r).x;
    float dyl = textureOffset(tex, uv, l.yx).x;
    float dyr = textureOffset(tex, uv, r.yx).x;

    vec2 texSize = textureSize(tex, 0);

    vec2 derx = vec2(2.0/texSize.x, 0);
    vec2 dery = vec2(0, 2.0/texSize.y);

    vec3 dx = vec3(derx, (dxr - dxl));
    vec3 dy = vec3(dery, (dxr - dxl));

    vec3 normal_mv = normalize(cross(dx,dy));

    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0 : nl;

    color = colorHeight*(nl*Ld + La);
    //color = vec3(1, 2/texSize.x * 2048, 0);
}
