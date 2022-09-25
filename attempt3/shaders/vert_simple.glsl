#version 330 core

layout (location = 0) in vec3 iposition;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexcoord;

//在世界空间中的顶点位置和法向量
out vec3 wPosition;
out vec3 wNormal;

out vec2 texCoord;

uniform mat4 model_matrix;      //模型矩阵
uniform mat4 mv_matrix;         //模型矩阵*相机位置(观察)(model*view)矩阵
uniform mat4 proj_matrix;       //投影(projection)矩阵
uniform mat3 normal_matrix;     //法向量的修正矩阵


void main(){
    gl_Position = proj_matrix * mv_matrix * vec4(iposition, 1.0);
    wPosition = vec3(model_matrix * vec4(iposition, 1.0));
    wNormal = normal_matrix * inormal;
    texCoord = itexcoord;
    //vertcolor = vec3(0.8, 0.8, 0.8);
    
    //vertcolor = vec4(normalize(position), 1.0);
    //vertcolor = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}