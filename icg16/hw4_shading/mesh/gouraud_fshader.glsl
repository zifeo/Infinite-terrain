#version 330


out vec3 color;
in vec3 vcolor;

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

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 0.1: Gouraud shading.

    color = vcolor;

    //<<<<<<<<<< TODO <<<<<<<<<<<
}
