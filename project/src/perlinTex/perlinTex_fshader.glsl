#version 330 core


#define RANDNBR 256
#define SIZE coef * RANDNBR
#define MAXOCTAVE 4

out vec3 color;
in vec2 tpoint;
uniform sampler1D gradientMap;
uniform int Perm[RANDNBR];
uniform float lac;
uniform float H;
uniform int octave;

vec2 tpoint_1 = tpoint;


float perl_mix(float x, float y, float a) {
    return (1-a)*x + a*y;
}

float gradAndDot(float rand, vec2 p) {
    return dot(texture(gradientMap, rand).xy, p);
}

float smoothInt(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // reduces the number of multiplications
}

float perlin() {

    float size = RANDNBR;
    vec2 tpoint_sized = tpoint_1*size;
    vec2 tpoint_cell = (tpoint_sized - floor(tpoint_sized));
    vec2 tpoint_floor = floor(tpoint_sized);

    // 4 corners of the block
    vec2 LD = tpoint_floor;
    vec2 RD = tpoint_floor + vec2(1,0);
    vec2 RU = tpoint_floor + vec2(1,1);
    vec2 LU = tpoint_floor + vec2(0,1);

    float rand_LD = Perm[int(mod((Perm[int(mod(LD.x, RANDNBR))] + LD.y), RANDNBR))] / float(RANDNBR);
    float rand_RD = Perm[int(mod((Perm[int(mod(RD.x, RANDNBR))] + RD.y), RANDNBR))] / float(RANDNBR);
    float rand_RU = Perm[int(mod((Perm[int(mod(RU.x, RANDNBR))] + RU.y), RANDNBR))] / float(RANDNBR);
    float rand_LU = Perm[int(mod((Perm[int(mod(LU.x, RANDNBR))] + LU.y), RANDNBR))] / float(RANDNBR);

    float smoothx = smoothInt(tpoint_cell.x);

    float st = perl_mix(gradAndDot(rand_LD, tpoint_cell),
                        gradAndDot(rand_RD, tpoint_cell + vec2(-1,0)),
                        smoothx);

    float uv = perl_mix(gradAndDot(rand_LU, tpoint_cell + vec2(0,-1)),
                        gradAndDot(rand_RU, tpoint_cell + vec2(-1,-1)),
                        smoothx);

    return perl_mix(st, uv, smoothInt(tpoint_cell.y));
}

float noiseBFM(float H, float lacunarity, int octaves) {
    float value = 0.0;
    float max = 0.0;
     /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        max += pow(lacunarity, -H*i);
        value += perlin() * pow(lacunarity, -H*i);
        tpoint_1 *= lacunarity;
    }

    return value / max;
}

void main() {

    float noise = (noiseBFM(H, lac, octave) + 1)/2;

    color = vec3(tpoint, noise);
}

