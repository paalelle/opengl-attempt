#version 330 core

in vec3 wPosition;
in vec3 wNormal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D textureDiff1;
uniform sampler2D textureNorm1;
uniform sampler2D textureSpec1;

float ambientMinimum = 0.2;
float ambientStrength = 0.3;
float specularStrength = 0.3;           //高光强度系数
float shininess = 32.0;                 //高光范围系数

void main(){
    vec3 nlightDir = normalize(lightDir);
    vec3 nviewDir = normalize(cameraPos - wPosition);

    //漫反射分量系数
    float diffuseFactor = max(0.0, dot(-nlightDir, wNormal));

    //环境光分量系数
    float ambientFactor = max(ambientMinimum, ambientStrength * dot(vec3(0, -1.0, 0), wNormal));


    float mainLightingFactor;
    if(diffuseFactor + ambientFactor > 0.55) mainLightingFactor = 1.0;
    else if(diffuseFactor + ambientFactor > 0.45) mainLightingFactor = 0.75;
    else mainLightingFactor = 0.5;

    vec3 mainLighting = mainLightingFactor * lightColor;
    
    //Blinn-Phong
    //高光系数
    float specularFactor = max(0.0, dot(wNormal, normalize(nviewDir - lightDir)));
    specularFactor = specularStrength * pow(specularFactor, shininess);
    vec3 specular = specularFactor * vec3(texture(textureSpec1, texCoord)) * lightColor;


    vec3 baseColor = vec3(texture(textureDiff1, texCoord));

    vec3 color = (mainLighting + specular) * baseColor;


    fragColor = vec4(color, 1.0);
    //fragColor = baseColor;
    //fragColor = vec4(color, 1.0);
    //fragColor = vec4(nviewDir, 1.0);
}