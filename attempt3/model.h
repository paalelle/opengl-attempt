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

//�� https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/







class Model{
private:
    std::vector<Mesh> meshes;
    std::string directory;

    bool isValid;


    //����һ��node��������Ϣ
    //node: Assimp����������,ÿ��node������һ��������mesh��Ϣ,���������е�����node�ǲ㼶��ϵ
    void processNode(aiNode *node, const aiScene *scene);

    //����һ��mesh��������Ϣ
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    //����һ��mesh������������Ϣ
    std::vector<Texture> loadTextures(aiMaterial *aimat, aiTextureType aitype, TextureType ttype);

public:
    Model();
    Model(std::string path);

    bool IsLoaded();
    void Draw(Shader *dshader);


    void ImportModel(std::string path);

    
};


//ģ��������,���泡��������ͬ��ģ�͵�λ����Ϣ
class ModelLinker{
private:
    Model *pmodel;

    //����modelͳһʹ�õ�shader,��ѡ��
    Shader *mshader;


    //ģ�;�������
    //��Ӧ��ģ���ڳ����е����аڷ�λ��
    std::vector<glm::mat4> modelMatrices;

    
    glm::mat4 mvMatrix;         //ģ�;���͹۲����ĳ˻�,�ڻ���ʱ����
    glm::mat4 projMatrix;       //ͶӰ����,�ڻ���ʱ����
    glm::mat4 normalMatrix;     //��������������,�ڻ���ʱ����

public:
    ModelLinker();
    ModelLinker(Model *pm);
    ModelLinker(Model *pm, Shader *ps);

    void SetLinkedModel(Model *pm);
    void BindShader(Shader *dshader);

    glm::mat4 GetModelMat(unsigned index);
    
    //�Դ�ģ�ʹ���һ����λ��
    void NewModelMat();             //Ĭ��ģ�;���ΪI
    void NewModelMat(glm::mat4 m);
    
    //��ģ�;���任�ĺ���
    //��index��-1,������д�ģ�͵�ʵ����Ӧ��ģ�;���任
    void SetModelMat(int index, glm::mat4 m);
    void Rotate(int index, float yaw, float pitch, float roll);     //����������ת��ŷ����(ƫ����,������,�����)
    void Move(int index, glm::vec3 mvec);
    void Move(int index, float x, float y, float z);
    void Scale(int index, float val);
    void Scale(int index, float x, float y, float z);

    //ʹ��dshader��������ģ��
    void DrawAll(Shader *dshader, glm::mat4 mview, glm::mat4 mproj);

    //ʹ��mshader��������ģ��
    void DrawAll(glm::mat4 mview, glm::mat4 mproj);
};

