#version 420 core
layout (location = 0) in vec3 aPos;// the position variable has attribute position 0
layout (location = 1) in vec3 aColor;// the color variable has attribute position 1
layout (location = 2) in vec3 gradient;//vertex in list before current vertex
out vec3 fColor;// output a color to the fragment shader

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix *  uModelMatrix * vec4(aPos, 1.0);
    fColor = abs(gradient);// set ourColor to the input color we got from the vertex data
}