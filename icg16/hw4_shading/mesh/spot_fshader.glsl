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
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 5: Spot light.
    // Complete the shader to obtain spot light effect
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 proj = dot(light_dir, normal_mv) * normal_mv;
    vec3 r_dir = normalize(2 * proj - light_dir);

    float nl = dot(normal_mv, light_dir);
    nl = max(nl, 0);

    float rv = dot(r_dir, view_dir);
    rv = max(rv, 0);

    float spot_effect = 0;
    if (spot_effect_not_pow >= spot_cos_cutoff)
        spot_effect = pow(spot_effect_not_pow, spot_exp);
    else
        spot_effect = 0;

    color = ka * La + (kd * nl * Ld + ks * pow(rv, alpha) * Ls) * spot_effect;
}
