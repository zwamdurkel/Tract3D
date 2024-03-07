#version 420 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;
in vec4 modelPos;

uniform vec3 uViewPos;
uniform bool uDrawTubes;
uniform float alpha;

void main()
{
    vec3 lightColor = vec3(1, 1, 1);
    vec3 lightPos = vec3(0.702, 0.702, 0);

    //calculate normal
    vec3 g = normalize(normal);//gradient vector
    //make sure g is pointing upwards i guess
    g = sign(dot(g, vec3(0, 1, 0))) * g;

    vec3 p = vec3(modelPos);
    vec3 v = uViewPos - p;
    vec3 up = cross(g, v);
    vec3 n = normalize(cross(g, up));

    //make sure n is pointing towards camera
    n = sign(dot(v, n)) * n;
    float f = max(dot(n, lightPos), 0.25);

    FragColor = vec4(f * lightColor, alpha);
}