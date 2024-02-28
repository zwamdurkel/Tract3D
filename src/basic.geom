#version 420 core

layout(lines) in;
layout(line_strip) out;
layout(max_vertices = 2) out;

in vec3 ourColor[];
out vec3 fColor;

void main() {
    fColor = ourColor[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    fColor = ourColor[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    EndPrimitive();
}
