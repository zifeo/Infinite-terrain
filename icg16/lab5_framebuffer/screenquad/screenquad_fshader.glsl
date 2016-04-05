#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

void main() {
    /*mat3 gx = mat3(-1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f);
    mat3 gy = transpose(gx);

    float ix = 0;
    ix += gx[0][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, 1)).rgb);
    ix += gx[0][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, 1)).rgb);
    ix += gx[0][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, 1)).rgb);
    ix += gx[1][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, 0)).rgb);
    ix += gx[1][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, 0)).rgb);
    ix += gx[1][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, 0)).rgb);
    ix += gx[2][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, -1)).rgb);
    ix += gx[2][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, -1)).rgb);
    ix += gx[2][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, -1)).rgb);

    float iy = 0;
    iy += gy[0][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, 1)).rgb);
    iy += gy[0][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, 1)).rgb);
    iy += gy[0][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, 1)).rgb);
    iy += gy[1][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, 0)).rgb);
    iy += gy[1][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, 0)).rgb);
    iy += gy[1][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, 0)).rgb);
    iy += gy[2][0] * rgb_2_luma(textureOffset(tex, uv, ivec2(-1, -1)).rgb);
    iy += gy[2][1] * rgb_2_luma(textureOffset(tex, uv, ivec2(0, -1)).rgb);
    iy += gy[2][2] * rgb_2_luma(textureOffset(tex, uv, ivec2(1, -1)).rgb);

    float iout = 1.0 - sqrt(ix * ix + iy * iy);
    color = vec3(iout);*/

    const float sigma = 2;
    const int size = 11;

    float tot_weight = 0;
    vec3 tot_image = vec3(0);
    for(int i = -size/2; i <= size/2; i++){
        for(int j = -size/2; j <= size/2; j++){
            float dist = pow(i, 2) + pow(j, 2);
            float weight = exp(-dist/(2 * sigma * sigma)); // gaussian
            tot_image += weight * texture(tex, uv + vec2(i/tex_width,j/tex_height)).rgb;
            tot_weight += weight;
        }
    }
    color = tot_image / tot_weight;
}

