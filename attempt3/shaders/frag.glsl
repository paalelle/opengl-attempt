#version 330 core

in vec3 wPosition;
in vec3 wNormal;
out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

vec3 baseColor = vec3(0.5, 0.8, 0.9);
float specularStrength = 0.8;
float shininess = 32.0;

void main(){
    vec3 nlightDir = normalize(lightDir);
    vec3 nviewDir = normalize(viewPos - wPosition);

    float diffusek = max(0.0, dot(-nlightDir, wNormal));
    vec3 diffuse = diffusek * lightColor;

    float ambientk = max(0.2, 0.3 * dot(vec3(0, -1.0, 0), wNormal));
    vec3 ambient = ambientk * lightColor;

    float speculark = max(0.0, dot(wNormal, normalize(nviewDir - lightDir)));
    speculark = specularStrength * pow(speculark, shininess);
    vec3 specular = speculark * lightColor;

    vec3 color = (diffuse + specular + ambient) * baseColor;

    fragColor = vec4(color, 1.0);
    //fragColor = vec4(nviewDir, 1.0);
}