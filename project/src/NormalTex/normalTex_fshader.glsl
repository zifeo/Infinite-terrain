#version 330 core


#define RANDNBR 256
#define SIZE coef * RANDNBR
#define MAXOCTAVE 4

out vec4 color;
in vec2 tpoint;
uniform sampler2D tex_perlin;

void main() {
    vec3 perlinValue = texture(tex_perlin, tpoint.xy).xyz;

    vec3 dx = (vec3(dFdx(tpoint),dFdx(perlinValue.z)));
    vec3 dy = (vec3(dFdy(tpoint),dFdy(perlinValue.z)));

    vec3 normal = (normalize(cross(dx,dy)) + vec3(1))/2;

    color = vec4(perlinValue.z);
}

