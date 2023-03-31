#version 330 core

struct UMatrices{
    mat4 model;      //模型矩阵
    mat4 mv;         //模型矩阵*相机位置(观察)(model*view)矩阵
    mat4 proj;       //投影(projection)矩阵
    mat3 normal;     //法向量的修正矩阵
};

layout (location = 0) in vec3 iposition;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexcoord;

//在世界空间中的顶点位置和法向量
out vec3 wPosition;
out vec3 wNormal;
out vec2 texCoord;

uniform UMatrices umatrices;


void main(){
    gl_Position = umatrices.proj * umatrices.mv * vec4(iposition, 1.0);
    wPosition = vec3(umatrices.model * vec4(iposition, 1.0));
    wNormal = normalize(umatrices.normal * inormal);
    texCoord = itexcoord;
    //vertcolor = vec3(0.8, 0.8, 0.8);
    
    //vertcolor = vec4(normalize(position), 1.0);
    //vertcolor = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}