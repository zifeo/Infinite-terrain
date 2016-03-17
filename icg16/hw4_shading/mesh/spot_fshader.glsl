#version 330


out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
in float spot_effect_sns_exp;

void main() {    
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
    // TODO 5: Spot light.
    // Complete the shader to obtain spot light effect
    vec3 petit_n = dot(light_dir, normal_mv) * normal_mv;
    vec3 r = normalize((petit_n - light_dir) + petit_n);

    float nl = dot(normal_mv, light_dir);
    nl = nl < 0 ? 0 : nl;
    float rv = dot(r, view_dir);
    rv = rv < 0 ? 0 : rv;
    color = ka * La + kd * nl * Ld + ks * pow(rv, alpha) * Ls;

    float spot_effect = pow(spot_effect_sns_exp, spot_exp);

    color *= spot_effect;

    //<<<<<<<<<< TODO <<<<<<<<<<<
}
