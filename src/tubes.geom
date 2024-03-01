#version 420 core

layout(lines) in;
layout(triangle_strip) out;
layout(max_vertices = 16) out;

in vec3 ourColor[];
in vec4 modelPos[];
out vec3 fColor;
out vec3 normal;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void emitSide(vec4 delta){
    fColor = ourColor[0];
    normal = delta.xyz;
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[0] + delta);
    EmitVertex();
    fColor = ourColor[1];
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[1] + delta);
    EmitVertex();
}

void emitPrism(vec4 delta1, vec4 delta2, vec4 delta3, int index, vec3 newNormal)
{
    fColor = ourColor[index];
    normal = newNormal;
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta1);
    EmitVertex();
    fColor = ourColor[index];
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta2);
    EmitVertex();
    fColor = ourColor[index];
    normal = newNormal;
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta1);
    EmitVertex();
    fColor = ourColor[index];
    gl_Position = uProjectionMatrix * uViewMatrix * (modelPos[index] + delta3);
    EmitVertex();
}

void main() {
    const float tubesize = 0.1f;
    vec3 diff = normalize((modelPos[0] - modelPos[1]).xyz);
    vec3 corner1 = cross(diff, vec3(0, 1, 0));
    vec3 corner2 = cross(diff, corner1);
    vec3 corner3 = -normalize(corner1 + corner2);
    vec4 c1 = vec4(corner1 * tubesize, 0);
    vec4 c2 = vec4(corner2 * tubesize, 0);
    vec4 c3 = vec4(corner3 * tubesize, 0);

    emitPrism(c1, c2, c3, 0, diff);
    EndPrimitive();

    //side 1
    emitSide(c1);
    emitSide(c2);
    emitSide(c3);
    emitSide(c1);
    EndPrimitive();

    emitPrism(c3, c2, c1, 1, -diff);
    EndPrimitive();


}
