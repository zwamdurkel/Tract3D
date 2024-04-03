#version 460 core
out vec4 FragColor;
in vec3 fColor;
in vec3 normal;
in vec4 modelPos;
in float simInt;

uniform vec3 uViewPos;
uniform bool uDrawTubes;
uniform float alpha;
uniform vec3 lightDir;

uniform bool neuronSim;
uniform float time;
uniform bool blackSim;
uniform int particleDens;

void main()
{
    //    const vec3 ambColor = vec3(0.7, 0.2, 0.2);
    //    const vec3 diffColor = vec3(0.7, 0.2, 0.2);
    vec3 ambColor = fColor;
    vec3 diffColor = fColor;
    const vec3 specColor = vec3(1, 1, 1);
    //const vec3 lightDir = vec3(0.702, 0.702, 0);

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
    float diff = max(dot(n, lightDir), 0);
    vec3 diffuse = diff * diffColor;

    //specular reflections
    const float specularStrength = 0.3;
    const float shininess = 2.3f;
    vec3 reflectDir = reflect(-lightDir, n);
    vec3 halfwayDir = normalize(lightDir + v);
    float spec = pow(max(dot(n, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * specColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, alpha);

    if (neuronSim){
        int t = int(floor(time)) / (particleDens - 1);
        float tmod10 = time - t*(particleDens - 1);
        bool simCol = abs((tmod10) - simInt) < 0.05f;
        if (!simCol && blackSim){ discard; }
        if (simCol){
            FragColor = vec4(vec3(simCol), 1);
        }
    }
}