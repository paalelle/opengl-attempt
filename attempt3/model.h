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
#include "mesh.h"

//见 https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/







class Model{
private:
    std::vector<Mesh> meshes;
    std::string directory;

    bool isValid;


    //处理一个node的所有信息
    //node: Assimp中数据类型,每个node包含了一定数量的mesh信息,整个场景中的所有node是层级关系
    void processNode(aiNode *node, const aiScene *scene);

    //处理一个mesh的所有信息
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    //处理一个mesh中所有纹理信息
    std::vector<Texture> loadTextures(aiMaterial *aimat, aiTextureType aitype, TextureType ttype);

public:
    Model();
    Model(std::string path);

    bool IsLoaded();
    void Draw(Shader *dshader);


    void ImportModel(std::string path);

    
};


//模型连接类,保存场景中所有同种模型的位置信息
class ModelLinker{
private:
    Model *pmodel;

    //整个model统一使用的shader,可选用
    Shader *mshader;


    //模型矩阵序列
    //对应此模型在场景中的所有摆放位置
    std::vector<glm::mat4> modelMatrices;

    
    glm::mat4 mvMatrix;         //模型矩阵和观察矩阵的乘积,在绘制时传入
    glm::mat4 projMatrix;       //投影矩阵,在绘制时传入
    glm::mat4 normalMatrix;     //法向量修正矩阵,在绘制时计算

public:
    ModelLinker();
    ModelLinker(Model *pm);
    ModelLinker(Model *pm, Shader *ps);

    void SetLinkedModel(Model *pm);
    void BindShader(Shader *dshader);

    glm::mat4 GetModelMat(unsigned index);
    
    //对此模型创建一个新位置
    void NewModelMat();             //默认模型矩阵为I
    void NewModelMat(glm::mat4 m);
    
    //对模型矩阵变换的函数
    //若index填-1,则对所有此模型的实例对应的模型矩阵变换
    void SetModelMat(int index, glm::mat4 m);
    void Rotate(int index, float yaw, float pitch, float roll);     //绕三个轴旋转的欧拉角(偏航角,俯仰角,横滚角)
    void Move(int index, glm::vec3 mvec);
    void Move(int index, float x, float y, float z);
    void Scale(int index, float val);
    void Scale(int index, float x, float y, float z);

    //使用dshader绘制所有模型
    void DrawAll(Shader *dshader, glm::mat4 mview, glm::mat4 mproj);

    //使用mshader绘制所有模型
    void DrawAll(glm::mat4 mview, glm::mat4 mproj);
};

