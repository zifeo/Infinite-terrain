#version 330

#define TEXSIZE 1024

in vec2 uv;
in vec3 light_dir;
in float height;

out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D tex;
uniform sampler2D sand_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;

const ivec2 l = ivec2(-1,0);
const ivec2 r = ivec2(1,0);

float coeffSand(float height, float angle) { // angle => 0 = plat, 1 = falaise
    return clamp((1 - angle) * (1 - 25 * (height-0.1)*(height-0.1)), 0, 1);
}

float coeffGrass(float height, float angle) {
    return clamp((1 - angle) * (1 - 25 * (height-0.35)*(height-0.35)), 0, 1);
}

float coeffRock(float height, float angle) {
    return clamp(angle + 1 * (1 - 225 * (height-0.58)*(height-0.58)), 0, 1);
}

float coeffSnow(float height, float angle) {
    return clamp((1 - angle) * (1 - 25 * (height-0.7)*(height-0.7)), 0, 1);
}

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

    float angle = abs(normal_mv.y)*abs(normal_mv.y);
    float cSand = coeffSand(height, angle);
    float cGrass = coeffGrass(height, angle);
    float cRock = coeffRock(height, angle);
    float cSnow = coeffSnow(height, angle);

    float sum = cSand + cGrass + cRock + cSnow;

    vec3 colorTex = (cSand * texture(sand_tex, uv).rgb + cGrass * texture(grass_tex, uv).rgb + cRock * texture(rock_tex, uv).rgb + cSnow * texture(snow_tex, uv).rgb)/sum;

    color = colorTex*(nl*Ld + La);
    //color = vec3(1, 2/texSize.x * 2048, 0);
}
