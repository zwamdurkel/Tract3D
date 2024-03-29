#version 460 core
layout (location = 0) in vec3 aPos;// the position variable has attribute position 0
layout (location = 1) in vec3 color;//vertex in list before current vertex
layout (location = 2) in vec3 n;//vertex in list before current vertex

out vec3 fColor;// output a color to the fragment shader
out vec3 normal;
out vec4 modelPos;//real world coordinate for geometry shader

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    normal = vec3(uModelMatrix * vec4(n, 1.0));
    modelPos = uModelMatrix * vec4(aPos, 1.0);
    gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
    fColor = abs(vec3(uModelMatrix * vec4(color, 1.0)));// set ourColor to the input color we got from the vertex data
}