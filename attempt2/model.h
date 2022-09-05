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

#include "shader.h"
#include "camera.h"



//���ڶ���Ľṹ��
struct Vertex{
    glm::vec3 Position;     //����λ��
    glm::vec3 Normal;       //������
    glm::vec2 TexCoords;    //�����Ӧ��������

    //debug
    void PrintData();
};

//������
class Mesh{
private:
    GLuint VAO, VBO, EBO;   //��Ӧ�������VAO,VBO,EBO

    std::vector<Vertex> vertices;     //������Ϣ
    std::vector<GLuint> indices;      //����EBO�ĸ������εĶ���������Ϣ

    bool isValid;
public:
    Mesh();

    bool IsLoaded();
    void SetGLInfo();
    void Load(std::vector<Vertex> vert, std::vector<GLuint> ind);
    void Draw(Shader *dshader);
    

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

    //����������ת��ŷ����(ƫ����,������,�����)
    void Rotate(float yaw, float pitch, float roll);
    void Move(glm::vec3 mvec);
    void Move(float x, float y, float z);

    void Draw(Shader *dshader);
};


class Model{

};

