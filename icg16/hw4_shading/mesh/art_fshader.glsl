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
    color = vec3(0.0, 0.0, 0.0);

    const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0 : nl;
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0 : rv;
    color = ka * La + texture(tex2D, vec2(nl, pow(rv, alpha))).rgb;
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
