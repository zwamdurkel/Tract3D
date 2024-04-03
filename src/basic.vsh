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

layout(std430, binding = 3) readonly buffer colorBuffer
{
    ssboUnit ssboData[];
};

layout(std430, binding = 4) readonly buffer disBuffer
{
    float disData[];
};

out vec3 fColor;// output a color to the fragment shader
out vec3 normal;
out vec4 modelPos;//real world coordinate for geometry shader

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform bool uDrawTubes;
uniform bool uDrawCaps;
uniform bool uSmoothCap;
uniform int uNrOfSides;
uniform float uTubeDiameter;

// fixed cos lookup table
const float fc[9][8] = float[][](
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
const float fs[9][8] = float[][](
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
        if (uDrawCaps) {
            int vi = gl_VertexID / uNrOfSides;
            vec3 v = vec3(ssboData[vi].x, ssboData[vi].y, ssboData[vi].z);
            vec3 v2 = vec3(ssboData[vi + 1].x, ssboData[vi + 1].y, ssboData[vi + 1].z);
            vec3 r = vec3(ssboData[vi].gx, ssboData[vi].gy, ssboData[vi].gz);
            vec3 tmp = cross(v2 - v, r);
            int corner = gl_VertexID - vi * uNrOfSides;
            int rot = 0;

            if (corner % 2 == 0) {
                rot = corner / 2;
            } else {
                rot = uNrOfSides - (corner + 1) / 2;
            }

            if (tmp.x > -0.1 && tmp.x < 0.1) {
                rot = uNrOfSides - rot - 1;
            }

            vec3 perp = normalize(cross(r, vec3(0.0, 1.0, 0.0))) * uTubeDiameter;
            vec3 q = (1.0-fc[uNrOfSides][rot]) * dot(perp, r) * r + fc[uNrOfSides][rot] * perp + fs[uNrOfSides][rot] * cross(r, perp);

            if (uSmoothCap) {
                normal = vec3(uModelMatrix * vec4(q, 1.0));
            } else {
                normal = vec3(uModelMatrix * vec4(r, 1.0));
            }
            modelPos = uModelMatrix * vec4(q + v + vec3(disData[vi * 3], disData[vi * 3 + 1], disData[vi * 3 + 2]), 1.0);
            gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
            fColor = abs(vec3(uModelMatrix * vec4(r, 1.0)));
        } else {
            int nrOfVertices = uNrOfSides * 2 + 2;// total number of vertices per line segment
            int rotationMult = (gl_VertexID % nrOfVertices / 2) % uNrOfSides;// which corner are we
            int vi = gl_VertexID / nrOfVertices + gl_VertexID % 2;// index of vertex in SSBO
            vec3 v = vec3(ssboData[vi].x, ssboData[vi].y, ssboData[vi].z);// vertex
            vec3 r = vec3(ssboData[vi].gx, ssboData[vi].gy, ssboData[vi].gz);// gradient (rotation axis)

            // perpendicular to gradient
            vec3 perp = normalize(cross(r, vec3(0.0, 1.0, 0.0))) * uTubeDiameter;

            // rotated perpendicular
            vec3 q = (1.0-fc[uNrOfSides][rotationMult]) * dot(perp, r) * r + fc[uNrOfSides][rotationMult] * perp + fs[uNrOfSides][rotationMult] * cross(r, perp);

            normal = vec3(uModelMatrix * vec4(q, 1.0));
            modelPos = uModelMatrix * vec4(q + v + vec3(disData[vi * 3], disData[vi * 3 + 1], disData[vi * 3 + 2]), 1.0);
            gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
            fColor = abs(vec3(uModelMatrix * vec4(ssboData[vi].gx, ssboData[vi].gy, ssboData[vi].gz, 1.0)));
        }
    } else {
        normal = vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0));
        modelPos = uModelMatrix * vec4(ssboData[gl_VertexID].x + disData[gl_VertexID * 3], ssboData[gl_VertexID].y + disData[gl_VertexID * 3 + 1], ssboData[gl_VertexID].z + disData[gl_VertexID * 3 + 2], 1.0);
        gl_Position = uProjectionMatrix * uViewMatrix * modelPos;
        fColor = abs(vec3(uModelMatrix * vec4(ssboData[gl_VertexID].gx, ssboData[gl_VertexID].gy, ssboData[gl_VertexID].gz, 1.0)));
    }
}