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
uniform bool uDrawTubes;
uniform int uNrOfSides;
uniform float uTubeDiameter;

// fixed cos lookup table
float fc[9][8] = float[][](
float[](1.0, 0, 0, 0, 0, 0, 0, 0), // 0
float[](1.0, 0, 0, 0, 0, 0, 0, 0), // 1
float[](1.0, 0, 0, 0, 0, 0, 0, 0), // 2
float[](1.0, -0.5, -0.5, 0, 0, 0, 0, 0), // 3
float[](1.0, 0.0, -1.0, 0.0, 0, 0, 0, 0), // 4
float[](1.0, 0.30901699437494745, -0.8090169943749473, -0.8090169943749473, 0.30901699437494745, 0, 0, 0), // 5
float[](1.0, 0.5, -0.5, -1.0, -0.5, 0.5, 0, 0), // 6
float[](1.0, 0.6234898018587336, -0.22252093395631434, -0.900968867902419, -0.900968867902419, -0.22252093395631434, 0.6234898018587336, 0), // 7
float[](1.0, 0.7071067811865476, 0.0, -0.7071067811865475, -1.0, -0.7071067811865475, 0.0, 0.7071067811865476)// 8
);

// fixed sin lookup table
float fs[9][8] = float[][](
float[](0.0, 0, 0, 0, 0, 0, 0, 0), // 0
float[](0.0, 0, 0, 0, 0, 0, 0, 0), // 1
float[](0.0, 0, 0, 0, 0, 0, 0, 0), // 2
float[](0.0, -0.8660254037844387, 0.8660254037844385, 0, 0, 0, 0, 0), // 3
float[](0.0, -1.0, 0.0, 1.0, 0, 0, 0, 0), // 4
float[](0.0, -0.9510565162951535, -0.5877852522924732, 0.587785252292473, 0.9510565162951536, 0, 0, 0), // 5
float[](0.0, -0.8660254037844386, -0.8660254037844387, 0.0, 0.8660254037844385, 0.866025403784439, 0, 0), // 6
float[](0.0, -0.7818314824680298, -0.9749279121818236, -0.43388373911755823, 0.433883739117558, 0.9749279121818236, 0.7818314824680299, 0), // 7
float[](0.0, -0.7071067811865476, -1.0, -0.7071067811865476, 0.0, 0.7071067811865475, 1.0, 0.7071067811865476)// 8
);

void main()
{
    if (uDrawTubes) {
        int nrOfVertices = uNrOfSides * 2 + 2;// total number of vertices per line segment
        int rotationMult = (gl_VertexID % nrOfVertices / 2) % uNrOfSides;// which corner are we
        int vi = gl_VertexID / nrOfVertices + gl_VertexID % 2;// index of vertex in SSBO
        vec3 v = vec3(ssboData[vi].x, ssboData[vi].y, ssboData[vi].z);// vertex
        vec3 r = vec3(ssboData[vi].gx, ssboData[vi].gy, ssboData[vi].gz);// gradient (rotation axis)

        // perpendicular to gradient
        vec3 perp = normalize(cross(r, vec3(1.0, 0.0, 0.0))) * uTubeDiameter;

        // rotated perpendicular
        vec3 q = (1.0-fc[uNrOfSides][rotationMult]) * dot(perp, r) * r + fc[uNrOfSides][rotationMult] * perp + fs[uNrOfSides][rotationMult] * cross(r, perp);

        normal = vec3(uModelMatrix * vec4(q, 1.0));
        modelPos = uModelMatrix * vec4(q + v, 1.0);
        gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
        fColor = abs(vec3(uModelMatrix * vec4(ssboData[vi].gx, ssboData[vi].gy, ssboData[vi].gz, 1.0)));
    } else {
        normal = vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0));
        modelPos = uModelMatrix * vec4(ssboData[gl_VertexID].x, ssboData[gl_VertexID].y, ssboData[gl_VertexID].z, 1.0);
        gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
        fColor = abs(vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0)));
    }
}