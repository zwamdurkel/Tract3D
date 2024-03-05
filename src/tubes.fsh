#version 420 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;
in vec3 FragPos;

uniform vec3 uViewPos;

void main()
{
    vec3 lightColor = vec3(1, 1, 1);
    vec3 lightPos = vec3(0.33333333, 0.66666666, 0.66666666);

    // ambient
    float ambientStrength = 0.0;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.5);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.3;
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 0.5);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * fColor;
    FragColor = vec4(result, 1.0);
}