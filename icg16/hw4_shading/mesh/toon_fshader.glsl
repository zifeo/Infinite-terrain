#version 330


out vec3 color;

uniform sampler1D tex1D;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = max(nl, 0);
    float rv = dot(r, view_dir);
    rv = max(rv, 0);
    /// 1) compute ambient term.
    vec3 amb = ka * La;
    /// 2) compute diffuse term using the texture sampler tex.
    vec3 dif = kd * texture(tex1D, nl).r * Ld;
    /// 3) compute specular term using the texture sampler tex.
    vec3 spec = Ls * texture(tex1D, pow(rv,alpha)).r * ks;

    color = amb + dif + spec;
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
