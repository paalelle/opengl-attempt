#version 330 core

struct TextureSet{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
};

in vec3 wPosition;
in vec3 wNormal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

//纹理数量
uniform int textureCount;
uniform TextureSet textures[3];

float ambientMinimum = 0.2;
float ambientStrength = 0.3;
float specularStrength = 0.8;           //高光强度系数
float shininess = 32.0;                 //高光范围系数

void main(){
    vec3 nlightDir = normalize(lightDir);
    vec3 nviewDir = normalize(cameraPos - wPosition);

    //漫反射分量系数
    float diffuseFactor = max(0.0, dot(-nlightDir, wNormal));

    //环境光分量系数
    float ambientFactor = max(ambientMinimum, ambientStrength * dot(vec3(0, -1.0, 0), wNormal));

    //主光照
    vec3 mainLighting = min(1.0, diffuseFactor + ambientFactor) * lightColor;

    //Blinn-Phong 高光
    float specularFactor = max(0.0, dot(wNormal, normalize(nviewDir - lightDir)));
    specularFactor = specularStrength * pow(specularFactor, shininess);
    vec3 specular = specularFactor * vec3(texture(textures[0].specular, texCoord)) * lightColor;


    vec3 baseColor = vec3(texture(textures[0].diffuse, texCoord));

    vec3 color = (mainLighting + specular) * baseColor;

    fragColor = vec4(color, 1.0);
    //fragColor = vec4(nviewDir, 1.0);
}