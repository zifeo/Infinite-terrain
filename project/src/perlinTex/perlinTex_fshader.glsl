#version 330 core


#define RANDNBR 256
#define SIZE coef * RANDNBR

out vec3 color;
in vec2 tpoint;
uniform sampler1D tex;
uniform int P[RANDNBR];


float perl_mix(float x, float y, float a) {
    return (1-a)*x + a*y;
}

float gradAndDot(float rand, vec2 p) {
    return dot(texture(tex, rand).xy, p);
}

float smoothInt(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); // reduces the number of multiplications
}

void main() {
    float coef = 0.01;
    vec2 tpoint_cell = (tpoint*SIZE - floor(tpoint*SIZE));

    // 4 corners of the block
    vec2 LD = vec2(floor(tpoint.x * SIZE), floor(tpoint.y * SIZE));
    vec2 RD = vec2((floor(tpoint.x * SIZE) + 1), floor(tpoint.y * SIZE));
    vec2 RU = vec2((floor(tpoint.x * SIZE) + 1), (floor(tpoint.y * SIZE) + 1));
    vec2 LU = vec2(floor(tpoint.x * SIZE), (floor(tpoint.y * SIZE) + 1));

    float rand_LD = P[int(mod((P[int(mod(LD.x, RANDNBR))] + LD.y), RANDNBR))] / float(RANDNBR);
    float rand_RD = P[int(mod((P[int(mod(RD.x, RANDNBR))] + RD.y), RANDNBR))] / float(RANDNBR);
    float rand_RU = P[int(mod((P[int(mod(RU.x, RANDNBR))] + RU.y), RANDNBR))] / float(RANDNBR);
    float rand_LU = P[int(mod((P[int(mod(LU.x, RANDNBR))] + LU.y), RANDNBR))] / float(RANDNBR);

    float st = perl_mix(gradAndDot(rand_LD, tpoint_cell),
                        gradAndDot(rand_RD, tpoint_cell + vec2(-1,0)),
                        smoothInt(tpoint_cell.x));

    float uv = perl_mix(gradAndDot(rand_LU, tpoint_cell + vec2(0,-1)),
                        gradAndDot(rand_RU, tpoint_cell + vec2(-1,-1)),
                        smoothInt(tpoint_cell.x));


    //float noise = smoothInt(tpoint_cell.x*255);
    float noise = (perl_mix(st, uv, smoothInt(tpoint_cell.y)) + 1)/2;

    color = vec3(noise, noise, noise);
    //color = vec3(noise > 1, noise < 0, 0);
}
