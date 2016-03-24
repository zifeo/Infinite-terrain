#version 330


out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 1.2: Phong shading.
    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = max(nl, 0);
    float rv = dot(r, view_dir);
    rv = max(rv, 0);
    color = ka * La + kd * nl * Ld + ks * pow(rv, alpha) * Ls;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
