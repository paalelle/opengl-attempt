#pragma once

#include <stdio.h>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include "shader.h"
#include "camera.h"



//关于顶点的结构体
struct Vertex{
    glm::vec3 Position;     //顶点位置
    glm::vec3 Normal;       //法向量
    glm::vec2 TexCoords;    //顶点对应纹理坐标

    //debug
    void PrintData();
};


enum TextureType{
    None = 0,       //未加载
    Albedo = 1      //漫反射贴图
};
//纹理类
class Texture{
private:
    GLuint id;
    TextureType type;
public:
    Texture();

    void SetGLInfo();
    void LoadFromFile(const char *path, TextureType t);
};


//网格类
class Mesh{
private:
    GLuint VAO, VBO, EBO;   //对应此网格的VAO,VBO,EBO

    std::vector<Vertex> vertices;     //顶点信息
    std::vector<GLuint> indices;      //关于EBO的各三角形的顶点索引信息
    
    Shader *mshader;

    bool isValid;
public:
    Mesh();

    bool IsLoaded();
    void SetGLInfo();
    void Load(std::vector<Vertex> vert, std::vector<GLuint> ind);
    void BindShader(Shader *dshader);
    void Draw(Shader *dshader);
    void Draw();
    

    void ImportModel(const char *path);
};

class MeshLinker{
private:
    Mesh *pmesh;

    glm::mat4 modelMatrix;
public:
    MeshLinker();
    MeshLinker(Mesh *pm);

    glm::mat4 GetModelMat();
    void SetModelMat(glm::mat4 m);
    void SetLinkedMesh(Mesh *pm);

    //绕三个轴旋转的欧拉角(偏航角,俯仰角,横滚角)
    void Rotate(float yaw, float pitch, float roll);
    void Move(glm::vec3 mvec);
    void Move(float x, float y, float z);

    void Draw(Shader *dshader);
    void Draw();
};


class Model{

};


