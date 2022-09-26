#include "world.h"



Scene::Scene(){
	projMat = viewMat = glm::mat4(1.0);
}


void Scene::Init(){
	mainCamera.SetCamera(glm::vec3(0.0f, 0.0f, 5.0f), PI_f, 0);

	ResetProjMatrix();

}



void Scene::NewShader(const char *vertex_file_path, const char *fragment_file_path, std::string name){
	Shader nshader(vertex_file_path, fragment_file_path, name);
	shaders.push_back(nshader);
}
void Scene::NewModel(std::string path, std::string name){
	Model nmodel(path, name);
	models.push_back(nmodel);
}

void Scene::NewModelInstance(size_t model_index){
	if(model_index >= models.size()){
		std::cout << "Warning: Scene::NewModelInstance中指定的模型下标超出范围" << std::endl;
		return;
	}

	ModelLinker nmodellinker(&models[model_index]);
	modelInstances.push_back(nmodellinker);
}
void Scene::NewModelInstance(std::string model_name){
	for(size_t i = 0; i < models.size(); i++){
		if(models[i].GetName() == model_name){
			ModelLinker nmodellinker(&models[i]);
			modelInstances.push_back(nmodellinker);
			return;
		}
	}

	std::cout << "Warning: Scene::NewModelInstance中指定名称的模型不存在" << std::endl;
}
void Scene::NewModelInstance(size_t model_index, std::string shader_name){
	if(model_index >= models.size()){
		std::cout << "Warning: Scene::NewModelInstance中指定的模型下标超出范围" << std::endl;
		return;
	}
	
	size_t shader_index = shaders.size() + 1;

	for(size_t i = 0; i < shaders.size(); i++){
		if(shaders[i].GetName() == shader_name){
			shader_index = i;
			break;
		}
	}
	if(shader_index == shaders.size() + 1){
		std::cout << "Warning: Scene::NewModelInstance中指定名称的shader不存在" << std::endl;
		return;
	}

	ModelLinker nmodellinker(&models[model_index], &shaders[shader_index]);
	modelInstances.push_back(nmodellinker);
}
void Scene::NewModelInstance(std::string model_name, std::string shader_name){
	size_t model_index = models.size() + 1, shader_index = shaders.size() + 1;

	for(size_t i = 0; i < models.size(); i++){
		if(models[i].GetName() == model_name){
			model_index = i;
			break;
		}
	}
	if(model_index == models.size() + 1){
		std::cout << "Warning: Scene::NewModelInstance中指定名称的模型不存在" << std::endl;
		return;
	}

	for(size_t i = 0; i < shaders.size(); i++){
		if(shaders[i].GetName() == shader_name){
			shader_index = i;
			break;
		}
	}
	if(shader_index == shaders.size() + 1){
		std::cout << "Warning: Scene::NewModelInstance中指定名称的shader不存在" << std::endl;
		return;
	}

	ModelLinker nmodellinker(&models[model_index], &shaders[shader_index]);
	modelInstances.push_back(nmodellinker);
}


ModelLinker *Scene::GetModelLinker(size_t index){
	if(index >= modelInstances.size()){
		std::cout << "Warning: Scene::GetModelLinker中指定的下标超出范围" << std::endl;
		return nullptr;
	}

	return &modelInstances[index];
}
ModelLinker *Scene::GetModelLinker(std::string model_name){
	for(size_t i = 0; i < modelInstances.size(); i++){
		if(modelInstances[i].pmodel->GetName() == model_name){
			return &modelInstances[i];
		}
	}

	std::cout << "Warning: Scene::GetModelLinker中指定名称的模型不存在" << std::endl;
	return nullptr;
}



void Scene::ResetProjMatrix(){
	projMat = glm::perspective(FOV, windowAspect, 0.1f, 1000.0f);
}



void Scene::InputDetect(GLFWwindow *window, float deltatime){

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	mainCamera.RotateCamera(xpos, ypos);

	//鼠标指针归位至窗口中央
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		mainCamera.MoveCamera_Forward(deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		mainCamera.MoveCamera_Forward(-deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		mainCamera.MoveCamera_Side(-deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		mainCamera.MoveCamera_Side(deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		mainCamera.MoveCamera_Vertical(deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		mainCamera.MoveCamera_Vertical(-deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		mainCamera.SpeedBoost(true);
	}
	else mainCamera.SpeedBoost(false);

	//这个看情况要不要移到main外面去
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}



void Scene::Render(){
	viewMat = mainCamera.GetViewMat();

	for(size_t i = 0; i < shaders.size(); i++){
		//把所有着色器通用的uniform变量传进去

		//相机观察位置
		shaders[i].SendUniform_Vec3(glm::value_ptr(mainCamera.GetPosition()), 1, "cameraPos");
		
		
		shaders[i].SendUniform_Vec3(0.5, -2.0, -1.0, "lightDir");
		shaders[i].SendUniform_Vec3(1.0, 1.0, 1.0, "lightColor");
	}

	for(size_t j = 0; j < modelInstances.size(); j++){
		modelInstances[j].DrawAll(viewMat, projMat);
	}
}


