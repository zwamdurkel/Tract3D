#version 420 core
layout (location = 0) in vec3 aPos;// the position variable has attribute position 0
layout (location = 1) in vec3 aColor;// the color variable has attribute position 1
layout (location = 2) in vec3 gradient;//vertex in list before current vertex

out vec3 ourColor;// output a color to the fragment shader
out vec4 modelPos;//real world coordinate for geometry shader

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    modelPos = uModelMatrix * vec4(aPos, 1.0);
    gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
    ourColor = gradient;// set ourColor to the input color we got from the vertex data
}