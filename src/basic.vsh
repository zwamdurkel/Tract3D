#version 460 core
//layout (location = 0) in vec3 aPos;// the position variable has attribute position 0
//layout (location = 1) in vec3 color;//vertex in list before current vertex
//layout (location = 2) in vec3 n;//vertex in list before current vertex

struct ssboUnit {
    float x;
    float y;
    float z;
    float gx;
    float gy;
    float gz;
};

layout(std430, binding = 3) buffer colorBuffer
{
    ssboUnit ssboData[];
};

out vec3 fColor;// output a color to the fragment shader
out vec3 normal;
out vec4 modelPos;//real world coordinate for geometry shader

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
    normal = vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0));
    modelPos = uModelMatrix * vec4(ssboData[gl_VertexID].x, ssboData[gl_VertexID].y, ssboData[gl_VertexID].z, 1.0);
    gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
    //    fColor = abs(vec3(uModelMatrix * vec4(color, 1.0)));// set ourColor to the input color we got from the vertex data
    fColor = abs(vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0)));
}