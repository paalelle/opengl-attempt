#version 420 core

layout (location = 0) in vec2 aPositions;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

void main(){
	gl_Position = vec4(aPositions, 0.0, 1.0);
	texCoords = aTexCoords;
}