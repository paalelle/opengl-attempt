#version 420 core

uniform sampler2D texAverage;
uniform sampler2D texNew;
uniform float sampleCount;

in vec2 texCoords;
out vec4 fragColor;

void main(){
    vec4 averageColor = texture(texAverage, texCoords);
    vec4 newColor = texture(texNew, texCoords);
    //fragColor = vec4(texCoords, 1.0, 1.0);
    if(sampleCount < -0.1) fragColor = averageColor;       //用于输出固定后的图像
    else if(sampleCount < 1) fragColor = newColor;
    else fragColor = (averageColor * sampleCount + newColor) / (sampleCount + 1.0);

    //fragColor = averageColor;
    //fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}