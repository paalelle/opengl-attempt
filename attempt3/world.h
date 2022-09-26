#pragma once


#include "shader.h"
#include "model.h"
#include "camera.h"



//����,���������һϵ����ɫ����ģ�ͺ�Ϊ��Щģ��ָ����ģ�;��󹹳�
//����ģ�Ͳ��ܹ�����ģ��
class Scene{
private:
	std::vector<Shader> shaders;
	std::vector<Model> models;
	
	Model skyBox;

	Camera mainCamera;



	glm::mat4 projMat, viewMat;
	
public:
	std::vector<ModelLinker> modelInstances;


	Scene();

	void Init();

	
	//����һ����shader
	void NewShader(const char *vertex_file_path, const char *fragment_file_path, std::string name);
	//����һ����ģ��,��δָ��ģ��ʵ��
	void NewModel(std::string path, std::string name);
	
	//�½�һ��ģ��ʵ��
	void NewModelInstance(size_t model_index);
	void NewModelInstance(std::string model_name);
	void NewModelInstance(size_t model_index, std::string shader_name);
	void NewModelInstance(std::string model_name, std::string shader_name);

	ModelLinker *GetModelLinker(size_t index);
	ModelLinker *GetModelLinker(std::string model_name);


	void ResetProjMatrix();


	//������������
	//deltatimeΪ���ε���֡���º����ļ��ʱ��
	void InputDetect(GLFWwindow *window, float deltatime);

	//��Ⱦ��������
	void Render();
};
