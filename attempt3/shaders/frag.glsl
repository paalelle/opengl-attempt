#version 330 core

in vec3 wPosition;
in vec3 wNormal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D textureDiff1;
uniform sampler2D textureNorm1;
uniform sampler2D textureSpec1;

float specularStrength = 2.0;           //高光强度系数
float shininess = 32.0;                 //高光范围系数

void main(){
    vec3 nlightDir = normalize(lightDir);
    vec3 nviewDir = normalize(viewPos - wPosition);

    float diffuseFactor = max(0.0, dot(-nlightDir, wNormal));
    vec3 diffuse = diffuseFactor * lightColor;

    float ambientFactor = max(0.6, 1.0 * dot(vec3(0, -1.0, 0), wNormal));
    vec3 ambient = ambientFactor * lightColor;

    //Blinn-Phong
    //高光系数
    float specularFactor = max(0.0, dot(wNormal, normalize(nviewDir - lightDir)));
    specularFactor = specularStrength * pow(specularFactor, shininess);
    vec3 specular = specularFactor * vec3(texture(textureSpec1, texCoord)) * lightColor;

    //vec3 specular = speculark * lightColor;

    vec3 baseColor = vec3(texture(textureDiff1, texCoord));

    vec3 color = (diffuse + specular + ambient) * baseColor;


    fragColor = vec4(color, 1.0);
    //fragColor = baseColor;
    //fragColor = vec4(color, 1.0);
    //fragColor = vec4(nviewDir, 1.0);
}