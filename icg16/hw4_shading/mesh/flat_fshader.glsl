#version 330


out vec3 color;

//in vec3 normal_mv;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    color = vec3(0.0,0.0,0.0);
    const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];

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
    nl = nl < 0 ? 0 : nl;
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0 : rv;
    color = ka * La + kd * nl * Ld + ks * pow(rv, alpha) * Ls;
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
