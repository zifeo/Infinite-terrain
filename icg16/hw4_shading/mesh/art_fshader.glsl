#version 330


out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform sampler2D tex2D;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = max(nl, 0);
    float rv = dot(r, view_dir);
    rv = max(rv, 0);
    color = ka * La + texture(tex2D, vec2(nl, pow(rv, alpha))).rgb;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
