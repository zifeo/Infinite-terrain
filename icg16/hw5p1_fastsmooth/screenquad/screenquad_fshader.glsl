#version 330 core
uniform sampler2D tex_PASS1;
uniform sampler2D tex_PASS2;
uniform float tex_width;
uniform float tex_height;
vec3 TEX_PASS1(vec2 uv){ return texture(tex_PASS1, uv).rgb; }
vec3 TEX_PASS2(vec2 uv){ return texture(tex_PASS2, uv).rgb; }

uniform float std;
uniform int pass_nb; /// first v.s. second pass
in vec2 uv;

out vec3 color;

void main() {
    int size = 1 + 4*int(ceil(std*std));
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;

    if(pass_nb == 1) {
        for(int i = -size; i <= size; ++i) {
            float w = exp(-(i*i)/(2.0*std*std*std*std));
            vec3 neigh_color = TEX_PASS1( uv+vec2(i/tex_width, 0) );
            color_tot += w * neigh_color;
            weight_tot += w;
        }
        color = color_tot / weight_tot; 
        // color = TEX_PASS1( uv ); ///< debug
    } 
    else if(pass_nb == 2) {
        for(int i = -size; i <= size; ++i) {
            float w = exp(-(i*i)/(2.0*std*std*std*std));
            vec3 neigh_color = TEX_PASS2( uv+vec2(0, i/tex_height) );
            color_tot += w * neigh_color;
            weight_tot += w;
        }
        color = color_tot / weight_tot;    
        // color = TEX_PASS2( uv ); ///< debug
    }
}

