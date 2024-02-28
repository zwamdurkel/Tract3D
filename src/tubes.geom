#version 420 core

layout(lines) in;
layout(triangle_strip) out;
layout(max_vertices = 10) out;

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

void main() {
    const float tubesize = 0.1f;
    vec3 diff = normalize((modelPos[0] - modelPos[1]).xyz);
    vec3 corner1 = cross(diff, vec3(0, 1, 0));
    vec3 corner2 = cross(diff, corner1);
    vec3 corner3 = -normalize(corner1 + corner2);
    //imma do something illegal
    //side 1
    emitSide(vec4(corner1 * tubesize, 0));
    emitSide(vec4(corner2 * tubesize, 0));
    emitSide(vec4(corner3 * tubesize, 0));
    emitSide(vec4(corner1 * tubesize, 0));

    EndPrimitive();
}
