#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

const int N = 90;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)

    vec2 disp = 3*texture(velocityTex, uv).xy;
    color = texture(colorTex, uv);

    for (int i = 1; i < N; i++) {
        vec2 dispOffset = disp * float(i)/float(N-1);
        color += texture(colorTex, uv - dispOffset);
    }

    color /= N;
}
