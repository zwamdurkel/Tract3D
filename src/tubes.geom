#version 420 core

layout(lines) in;
layout(triangle_strip) out;
layout(max_vertices = 16) out;

in vec3 ourColor[];
in vec4 modelPos[];
in vec3 grad[];

out vec3 fColor;
out vec3 normal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void emitSide(vec4 delta, vec4 delta2){
    fColor = ourColor[0];
    normal = delta.xyz;
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[0] + delta);
    EmitVertex();
    fColor = ourColor[1];
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[1] + delta2);
    EmitVertex();
}

void emitPrism(vec4 delta1, vec4 delta2, vec4 delta3, int index, vec3 newNormal)
{
    fColor = ourColor[index];
    normal = newNormal;
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta1);
    EmitVertex();
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta3);
    EmitVertex();
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta2);
    EmitVertex();
}

void main() {
    const float tubesize = 0.1f;
    vec3 diff = normalize((modelPos[1] - modelPos[0]).xyz);
    //side 1
    vec3 ucorner1 = cross(grad[0], vec3(0, 1, 0));
    // Rodrigues' rotation formula
    vec3 ucorner2 = 1.5*dot(ucorner1, grad[0])*grad[0] - 0.5*ucorner1 + 0.86602540378*cross(grad[0], ucorner1);
    vec3 ucorner3 = 1.5*dot(ucorner2, grad[0])*grad[0] - 0.5*ucorner2 + 0.86602540378*cross(grad[0], ucorner2);
    vec4 uc1 = vec4(ucorner1 * tubesize, 0);
    vec4 uc2 = vec4(ucorner2 * tubesize, 0);
    vec4 uc3 = vec4(ucorner3 * tubesize, 0);

    //side 2
    vec3 lcorner1 = cross(grad[1], vec3(0, 1, 0));
    // Rodrigues' rotation formula
    vec3 lcorner2 = 1.5*dot(lcorner1, grad[1])*grad[1] - 0.5*lcorner1 + 0.86602540378*cross(grad[1], lcorner1);
    vec3 lcorner3 = 1.5*dot(lcorner2, grad[1])*grad[1] - 0.5*lcorner2 + 0.86602540378*cross(grad[1], lcorner2);
    vec4 lc1 = vec4(lcorner1 * tubesize, 0);
    vec4 lc2 = vec4(lcorner2 * tubesize, 0);
    vec4 lc3 = vec4(lcorner3 * tubesize, 0);

    // Only draw end caps
    if (cross(grad[0], diff).x <= 0.01) {
        emitPrism(uc1, uc2, uc3, 0, -grad[0]);
        EndPrimitive();
    }

    emitSide(uc1, lc1);
    emitSide(uc3, lc3);
    emitSide(uc2, lc2);
    emitSide(uc1, lc1);
    EndPrimitive();

    // Only draw end caps
    if (cross(grad[1], diff).x <= 0.01) {
        emitPrism(lc3, lc2, lc1, 1, grad[1]);
        EndPrimitive();
    }
}
