#pragma once


#include "shader.h"
#include "model.h"
#include "camera.h"



//场景,由摄像机、一系列着色器、模型和为这些模型指定的模型矩阵构成
//所有模型不能挂载子模型
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

	
	//创建一个新shader
	void NewShader(const char *vertex_file_path, const char *fragment_file_path, std::string name);
	//导入一个新模型,但未指定模型实例
	void NewModel(std::string path, std::string name);
	
	//新建一个模型实例
	void NewModelInstance(size_t model_index);
	void NewModelInstance(std::string model_name);
	void NewModelInstance(size_t model_index, std::string shader_name);
	void NewModelInstance(std::string model_name, std::string shader_name);

	ModelLinker *GetModelLinker(size_t index);
	ModelLinker *GetModelLinker(std::string model_name);


	void ResetProjMatrix();


	//处理所有输入
	//deltatime为两次调用帧更新函数的间隔时间
	void InputDetect(GLFWwindow *window, float deltatime);

	//渲染整个场景
	void Render();
};
