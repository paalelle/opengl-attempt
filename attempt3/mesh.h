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

//�� https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/



//���ڶ���Ľṹ��
struct Vertex{
    glm::vec3 Position;     //����λ��
    glm::vec3 Normal;       //������
    glm::vec2 TexCoords;    //�����Ӧ��������

    //debug
    void PrintData();
};



enum TextureType{
    texType_None = 0,       //δ����
    texType_Diffuse = 1,    //��������ͼ
    texType_Normal = 2,     //������ͼ
    texType_Specular = 3    //�߹���ͼ
};
//������
class Texture{
private:
    GLuint id;
    TextureType type;

    bool isValid;

    friend class Mesh;
    friend class Model;
    Texture();
    Texture(std::string path, TextureType t);

    //������id�����ô�����Ļ��ơ����˷�ʽ
    void SetGLInfo();

    //���ļ��м�������ͼƬ(�ȵ���SetGLInfo!)
    void LoadFromFile(std::string path, TextureType t);

};



//������
class Mesh{
private:
    GLuint VAO, VBO, EBO;   //��Ӧ�������VAO,VBO,EBO

    std::vector<Vertex> vertices;       //������Ϣ
    std::vector<GLuint> indices;        //����EBO�ĸ������εĶ���������Ϣ
    std::vector<Texture> textures;      //������Ϣ

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

//����������,�����Ӧ�����λ����Ϣ
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

    //����������ת��ŷ����(ƫ����,������,�����)
    void Rotate(float yaw, float pitch, float roll);
    void Move(glm::vec3 mvec);
    void Move(float x, float y, float z);

    void Draw(Shader *dshader);
};

