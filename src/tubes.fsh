#version 420 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;

void main()
{
    vec3 lightDir = vec3(10, 10, 0);
    lightDir = normalize(lightDir);
    vec3 n = normalize(normal);
    FragColor = vec4(fColor, 1.0) * max(0.5, dot(n, lightDir));
}