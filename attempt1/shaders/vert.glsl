#version 430

layout (location = 0) in vec3 position;

out vec4 vertcolor;

uniform mat4 mv_matrix;         //模型矩阵*相机位置(观察)(model*view)矩阵
uniform mat4 proj_matrix;       //投影(projection)矩阵 

void main(){
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    //vertcolor = vec4(normalize(position), 1.0);
    vertcolor = vec4(position,1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}