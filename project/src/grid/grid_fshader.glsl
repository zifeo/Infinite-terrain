#version 330

#define TEXSIZE 1024

in vec2 uv;
in vec3 light_dir;
in float height;
in float dist;

out vec4 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

uniform sampler2D tex;
uniform sampler2D sand_tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;
uniform float water_ampl;
uniform float time;

#define BIOME_COUNT 5

vec2 biome_position[BIOME_COUNT] =
    vec2[](vec2(0.5, 0.5), vec2(0.75, 0.35), vec2(0.35, 0.65), vec2(0.3, 0.2),
           vec2(0.5, 0.2)); // x -> temp, y -> altitude, if changes, need to copy to perlin shaders !

float coeffSand(float height, float angle, int biome) { // angle => 0 = plat, 1 = falaise
    switch (biome) {
    default:
        return clamp((1 - angle) * (1 - 25 * (height - 0.2) * (height - 0.2)), 0, 1);
    case 1:
        return 1;
    case 2:
        return 0;
    case 4:
        return 1;
    }
}

float coeffGrass(float height, float angle, int biome) {
    switch (biome) {
    default:
        return clamp((1 - angle) * (1 - 25 * (height - 0.5) * (height - 0.5)), 0, 1);
    case 1:
        return 0;
    case 2:
        return 0;
    }
}

float coeffRock(float height, float angle, int biome) {
    switch (biome) {
    default:
        return clamp(angle + 1 * (1 - 225 * (height - 0.58) * (height - 0.58)), 0, 1);
    case 1:
        return 0;
    case 2:
        return 0;
    }
}

float coeffSnow(float height, float angle, int biome) {
    switch (biome) {
    default:
        return clamp((1 - angle) * (1 - 25 * (height - 0.75) * (height - 0.75)), 0, 1);
    case 1:
        return 0;
    case 2:
        return 1;
    }
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

    if (uv.x <= texSize.x / (2 * texSize.x)) {
        zdx = textureOffset(tex, uv, r).x - perlinValue;
    } else {
        zdx = perlinValue - textureOffset(tex, uv, l).x;
    }

    if (uv.y <= texSize.x / (2 * texSize.x)) {
        zdy = textureOffset(tex, uv, r.yx).x - perlinValue;
    } else {
        zdy = perlinValue - textureOffset(tex, uv, l.yx).x;
    }

    vec3 dx = vec3(derx, abs(zdx));
    vec3 dy = vec3(dery, abs(zdy));

    vec3 normal_mv = normalize(cross(dx, dy));
    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0 : nl;

    float temperature = texture(tex, uv).y;
    float altitude = texture(tex, uv).z;

    float coeff_biomes[BIOME_COUNT];
    float sum = 0;
    for (int i = 0; i < BIOME_COUNT; i++) {
        float dist = (temperature - biome_position[i].x) * (temperature - biome_position[i].x) +
                     (altitude - biome_position[i].y) * (altitude - biome_position[i].y);

        if (dist != 0) {
            coeff_biomes[i] = 1 / (dist * dist * dist);
        } else {
            coeff_biomes[i] = 9999;
        }

        sum += coeff_biomes[i];
    }

    for (int i = 0; i < BIOME_COUNT; i++) {
        coeff_biomes[i] /= sum;
    }

    float angle = abs(normal_mv.y) * abs(normal_mv.y);

    float cSand = 0;
    float cGrass = 0;
    float cRock = 0;
    float cSnow = 0;
    for (int i = 0; i < BIOME_COUNT; i++) {
        cSand += coeff_biomes[i] * coeffSand(height, angle, i);
        cGrass += coeff_biomes[i] * coeffGrass(height, angle, i);
        cRock += coeff_biomes[i] * coeffRock(height, angle, i);
        cSnow += coeff_biomes[i] * coeffSnow(height, angle, i);
    }

    sum = cSand + cGrass + cRock + cSnow;

    vec3 colorTex = (cSand * texture(sand_tex, uv * 30).rgb + cGrass * texture(grass_tex, uv * 20).rgb +
                     cRock * texture(rock_tex, uv * 30).rgb + cSnow * texture(snow_tex, uv * 30).rgb) /
                    sum;

    vec3 color_temp3 = colorTex * (nl * Ld + La);
    if (height < 0.35 + water_ampl * sin(time) / 2) {
        color_temp3 = mix(color_temp3, vec3(0.2, 0.6, 1), 0.4);
    } else {
        color_temp3 = color_temp3;
    }

    vec4 brouillard = vec4(0.8, 0.8, 0.8, 1);
    color = mix(vec4(color_temp3, 1), brouillard, clamp(dist * dist / (9 * 9), 0, 1));
}
