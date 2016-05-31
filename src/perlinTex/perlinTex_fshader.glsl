#version 330 core

#define RANDNBR 256

out vec3 color;
in vec2 point;
uniform sampler1D gradientMap;
uniform int Perm[RANDNBR];
uniform int octaves;
uniform float lac;
uniform float H;
uniform int X;
uniform int Y;

uniform int temperature_octave;
uniform float temperature_lac;
uniform float temperature_H;

uniform int altitude_octave;
uniform float altitude_lac;
uniform float altitude_H;

#define BIOME_COUNT 5

vec2 biome_position[BIOME_COUNT] =
    vec2[](vec2(0.5, 0.5), vec2(0.75, 0.35), vec2(0.35, 0.65), vec2(0.3, 0.2),
           vec2(0.5, 0.2)); // x -> temp, y -> altitude, if changes, need to copy to grid shaders !

vec3 biome_parameter[BIOME_COUNT] = vec3[](vec3(H, lac, octaves), vec3(H, lac * 50, 2), vec3(H, lac, octaves),
                                           vec3(H, lac, octaves), vec3(H, lac, 1)); // x -> H, y -> lac, z -> octaves

float biome_amplitude[BIOME_COUNT] = float[](1, 0.3, 1.1, 1, 0.7);

float biome_offset[BIOME_COUNT] = float[](0, 0.3, 0.3, -0.3, -0.5);

float perl_mix(float x, float y, float a) { return (1 - a) * x + a * y; }

float gradAndDot(float rand, vec2 p) { return dot(texture(gradientMap, rand).xy, p); }

float smoothInt(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // reduces the number of multiplications
}

float perlin(vec2 point) {

    float size = RANDNBR;
    vec2 tpoint_sized = point;
    vec2 tpoint_cell = (tpoint_sized - floor(tpoint_sized));
    vec2 tpoint_floor = floor(tpoint_sized);

    // 4 corners of the block
    vec2 LD = tpoint_floor;
    vec2 RD = tpoint_floor + vec2(1, 0);
    vec2 RU = tpoint_floor + vec2(1, 1);
    vec2 LU = tpoint_floor + vec2(0, 1);

    float rand_LD = Perm[int(mod((Perm[int(mod(LD.x, RANDNBR))] + LD.y), RANDNBR))] / float(RANDNBR);
    float rand_RD = Perm[int(mod((Perm[int(mod(RD.x, RANDNBR))] + RD.y), RANDNBR))] / float(RANDNBR);
    float rand_RU = Perm[int(mod((Perm[int(mod(RU.x, RANDNBR))] + RU.y), RANDNBR))] / float(RANDNBR);
    float rand_LU = Perm[int(mod((Perm[int(mod(LU.x, RANDNBR))] + LU.y), RANDNBR))] / float(RANDNBR);

    float smoothx = smoothInt(tpoint_cell.x);

    float st = perl_mix(gradAndDot(rand_LD, tpoint_cell), gradAndDot(rand_RD, tpoint_cell + vec2(-1, 0)), smoothx);

    float uv = perl_mix(gradAndDot(rand_LU, tpoint_cell + vec2(0, -1)), gradAndDot(rand_RU, tpoint_cell + vec2(-1, -1)),
                        smoothx);

    return perl_mix(st, uv, smoothInt(tpoint_cell.y));
}

float noiseBFM(vec2 point, vec3 param) {
    float value = 0.0;
    float max = 0.0;
    float freq = 1.0;

    float H = param.x;
    float lacunarity = param.y;
    float octave = param.z;

    /* inner loop of fractal construction */
    for (int i = 0; i < octave; i++) {
        max += pow(freq, -H);
        value += (perlin(point) + 1) / 2 * pow(freq, -H);
        freq *= lacunarity;
        point *= lacunarity;
    }

    return value / max;
}

void main() {
    float temperature = 1.2*noiseBFM(point / 10, vec3(temperature_H, temperature_lac, temperature_octave));
    float altitude = noiseBFM((point + vec2(100)) / 10, vec3(altitude_H, altitude_lac, altitude_octave));

    float coeff_biomes[BIOME_COUNT];
    float sum = 0;
    for (int i = 0; i < BIOME_COUNT; i++) {
        float dist = (temperature - biome_position[i].x) * (temperature - biome_position[i].x) +
                     (altitude - biome_position[i].y) * (altitude - biome_position[i].y);

        if (dist * dist * dist != 0) {
            coeff_biomes[i] = 1 / (dist * dist * dist);
        } else {
            coeff_biomes[i] = 10000;
        }

        sum += coeff_biomes[i];
    }

    for (int i = 0; i < BIOME_COUNT; i++) {
        coeff_biomes[i] /= sum;
    }

    float noise = 0.0;
    for (int i = 0; i < BIOME_COUNT; i++) {
        noise += coeff_biomes[i] * (biome_amplitude[i] * noiseBFM(point / 1.4, biome_parameter[i]) + biome_offset[i]);
    }

    color = vec3(noise, temperature, altitude);
}
