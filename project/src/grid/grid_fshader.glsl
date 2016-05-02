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
uniform int x_chunk;

float coeffSand(float height, float angle) { // angle => 0 = plat, 1 = falaise
    return clamp((1 - angle) * (1 - 25 * (height - 0.2) * (height - 0.2)), 0, 1);
}

float coeffGrass(float height, float angle) {
    return clamp((1 - angle) * (1 - 25 * (height - 0.5) * (height - 0.5)), 0, 1);
}

float coeffRock(float height, float angle) {
    return clamp(angle + 1 * (1 - 225 * (height - 0.58) * (height - 0.58)), 0, 1);
}

float coeffSnow(float height, float angle) {
    return clamp((1 - angle) * (1 - 25 * (height-0.75)*(height-0.75)), 0, 1);
}

void main() {
    const ivec2 l = ivec2(-1, 0);
    const ivec2 r = ivec2(1, 0);

    vec2 texSize = textureSize(tex, 0);

    float perlinValue = texture(tex, uv).x;

    vec2 derx = vec2(1.0 / texSize.x, 0);
    vec2 dery = vec2(0, 1.0 / texSize.y);

    float zdx = 0;
    float zdy = 0;

    if (uv.x <= texSize.x / (2*texSize.x)) {
        zdx = textureOffset(tex, uv, r).x - perlinValue;
    } else {
        zdx = perlinValue - textureOffset(tex, uv, l).x;
    }

    if (uv.y <= texSize.x / (2*texSize.x)) {
        zdy = textureOffset(tex, uv, r.yx).x - perlinValue;
    } else {
        zdy = perlinValue - textureOffset(tex, uv, l.yx).x;
    }

    vec3 dx = vec3(derx, abs(zdx));
    vec3 dy = vec3(dery, abs(zdy));

    vec3 normal_mv = normalize(cross(dx, dy));
    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0 : nl;

    float angle = abs(normal_mv.y) * abs(normal_mv.y);
    float cSand = coeffSand(height, angle);
    float cGrass = coeffGrass(height, angle);
    float cRock = coeffRock(height, angle);
    float cSnow = coeffSnow(height, angle);

    float sum = cSand + cGrass + cRock + cSnow;

    vec3 colorTex = (cSand * texture(sand_tex, uv*30).rgb + cGrass * texture(grass_tex, uv*20).rgb +
                     cRock * texture(rock_tex, uv*30).rgb + cSnow * texture(snow_tex, uv*30).rgb) /
                    sum;

    color = colorTex * (nl * Ld + La);
}
