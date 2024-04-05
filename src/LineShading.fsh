#version 460 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;
in vec4 modelPos;
in float simInt;

uniform vec3 uViewPos;
uniform float uAlpha;
uniform vec3 uLightDir;
uniform bool uNeuronSim;
uniform float uTime;
uniform bool uBlackSim;
uniform int uParticleDens;
uniform float uParticleSize;

void main()
{
    vec3 ambColor = fColor;
    vec3 diffColor = fColor;
    const vec3 specColor = vec3(1, 1, 1);

    //calculate normal
    vec3 g = normalize(normal);//gradient vector
    //make sure g is pointing upwards i guess
    g = sign(dot(g, vec3(0, 1, 0))) * g;

    vec3 p = modelPos.xyz;
    vec3 v = normalize(uViewPos - p);
    vec3 up = cross(g, v);
    vec3 n = normalize(cross(g, up));

    //make sure n is pointing towards camera
    n = sign(dot(v, n)) * n;

    //ambient colour
    const float ambientFactor = 0.25f;
    vec3 ambient = ambientFactor * ambColor;

    //diffuse colour
    float diff = max(dot(n, uLightDir), 0);
    vec3 diffuse = diff * diffColor;

    //specular reflections
    const float specularStrength = 0.3;
    const float shininess = 2.3f;
    vec3 reflectDir = reflect(-uLightDir, n);
    vec3 halfwayDir = normalize(uLightDir + v);
    float spec = pow(max(dot(n, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * specColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, uAlpha);

    if (uNeuronSim){
        int t = int(floor(uTime)) / (uParticleDens - 1);
        float tmod10 = uTime - t*(uParticleDens - 1);
        bool simCol = abs((tmod10) - simInt) < uParticleSize;
        if (!simCol && uBlackSim){ discard; }
        if (simCol){
            FragColor = vec4(vec3(simCol), 1);
        }
    }
}