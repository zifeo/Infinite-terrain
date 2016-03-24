#version 330


out vec3 color;

//in vec3 normal_mv;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy

    /// 1) compute ambient term.
    /// 2) compute diffuse term.
    /// 3) compute specular term.

    vec3 x = dFdx(vpoint_mv).xyz;
    vec3 y = dFdy(vpoint_mv).xyz;
    vec3 normal_mv = normalize(cross(x,y));

    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = max(nl, 0);
    float rv = dot(r, view_dir);
    rv = max(rv, 0);
    color = ka * La + kd * nl * Ld + ks * pow(rv, alpha) * Ls;
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
