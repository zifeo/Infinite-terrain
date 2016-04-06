#version 330 core

uniform sampler2D tex_PASS1;
uniform float tex_width;
uniform float tex_height;
vec3 TEX_PASS(vec2 uv){ return texture(tex_PASS1, uv).rgb; } // only using tex_PASS_1

uniform float std;
in vec2 uv;

out vec3 color;

void main() {
    int size = 1 + 4*int(ceil(std*std));
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;

    for(int i = -size; i <= size; ++i) {
        for(int j = -size; j <= size; ++j) {
            float w = exp(-(i*i+j*j)/(2.0*std*std*std*std));
            vec3 neigh_color = TEX_PASS( uv+vec2(i/tex_width, j/tex_height) );
            color_tot += w * neigh_color;
            weight_tot += w;
        }
    }
    color = color_tot / weight_tot;

}

