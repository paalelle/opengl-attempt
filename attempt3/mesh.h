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

//见 https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/



//关于顶点的结构体
struct Vertex{
    glm::vec3 Position;     //顶点位置
    glm::vec3 Normal;       //法向量
    glm::vec2 TexCoords;    //顶点对应纹理坐标

    //debug
    void PrintData();
};



enum TextureType{
    texType_None = 0,       //未加载
    texType_Diffuse = 1,    //漫反射贴图
    texType_Normal = 2,     //法线贴图
    texType_Specular = 3    //高光贴图
};
//纹理类
class Texture{
private:
    GLuint id;
    TextureType type;

    bool isValid;

    friend class Mesh;
    friend class Model;
    Texture();
    Texture(std::string path, TextureType t);

    //绑定纹理id并设置此纹理的环绕、过滤方式
    void SetGLInfo();

    //从文件中加载纹理图片(先调用SetGLInfo!)
    void LoadFromFile(std::string path, TextureType t);

};



//网格类
class Mesh{
private:
    GLuint VAO, VBO, EBO;   //对应此网格的VAO,VBO,EBO

    std::vector<Vertex> vertices;       //顶点信息
    std::vector<GLuint> indices;        //关于EBO的各三角形的顶点索引信息
    std::vector<Texture> textures;      //纹理信息

    bool isValid;
public:
    Mesh();
    Mesh(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture> tex);

    bool IsLoaded();
    void SetGLInfo();
    void Load(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture> tex);
    void Draw(Shader *dshader);


    void ImportMesh(const char *path);
};

//网格连接类,保存对应网格的位置信息
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
};

