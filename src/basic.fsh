#version 460 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;
in vec4 modelPos;
in float simInt;

uniform vec3 uViewPos;
uniform bool uDrawTubes;
uniform float uAlpha;
uniform vec3 uLightDir;
uniform bool uNeuronSim;
uniform float uTime;
uniform bool uBlackSim;
uniform int uParticleDens;
uniform float uParticleSize;

void main()
{
    if (uDrawTubes) {
        vec3 lightColor = vec3(1, 1, 1);

        // ambient
        float ambientStrength = 0.0;
        vec3 ambient = ambientStrength * lightColor;

        // diffuse
        vec3 norm = normalize(normal);
        float diff = max(dot(norm, uLightDir), 0.5);
        vec3 diffuse = diff * lightColor;

        // specular
        float specularStrength = 0.3;
        vec3 viewDir = normalize(uViewPos - modelPos.xyz);
        vec3 reflectDir = reflect(-uLightDir, norm);
        vec3 halfwayDir = normalize(uLightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 0.5);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular) * fColor;
        FragColor = vec4(result, uAlpha);
    } else {
        FragColor = vec4(fColor, uAlpha);
    }

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