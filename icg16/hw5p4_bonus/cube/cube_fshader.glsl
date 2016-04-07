#version 330 core
uniform sampler2D tex;
in vec2 uv;
out vec3 color;
uniform sampler2D tex_mirror;

void main(){
    if (gl_PrimitiveID == 0 || gl_PrimitiveID == 1) {
        color = texture(tex_mirror, uv).rgb;
    } else {
        color = texture(tex, uv).rgb;
    }

}
