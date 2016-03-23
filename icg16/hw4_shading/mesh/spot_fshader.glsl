#version 330

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;
in float spot_effect_not_pow;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

out vec3 color;

const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

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
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 proj = dot(light_dir, normal_mv) * normal_mv;
    vec3 r_dir = normalize(2 * proj - light_dir);

    float nl = dot(normal_mv, light_dir);
    nl = nl >= 0 ? nl : 0;

    float rv = dot(r_dir, view_dir);
    rv = rv >= 0 ? rv : 0;

    float spot_effect = spot_effect_not_pow >= spot_cos_cutoff ? pow(spot_effect_not_pow, spot_exp) : 0;

    color = ka * La + (kd * nl * Ld + ks * pow(rv, alpha) * Ls) * spot_effect;
}
