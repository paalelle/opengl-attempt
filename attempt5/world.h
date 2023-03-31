#pragma once


#include "shader.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>



struct ObjSphere{
	glm::vec3 position;		//0	- base
	float radius;			//3
	glm::vec3 color;		//4
	float roughness;		//7
	float metallic;			//8
	float emission;			//9
	float tmpocc1;			//10 - 占位符1 (std140布局需要对齐4N)
	float tmpocc2;			//11 - 占位符2

	ObjSphere(){
		position = glm::vec3(0.0);
		radius = 1.0;
		color = glm::vec3(1.0);
		roughness = 0;
		metallic = 0;
		emission = 0;
	}
	ObjSphere(glm::vec3 p, float r, glm::vec3 co, float ro, float me, float em){
		position = p;
		radius = r;
		color = co;
		roughness = ro;
		metallic = me;
		emission = em;
	}
};							//12N in total

struct ObjPlane{
	glm::vec3 point;		//0 - base
	float tmpocc1;			//1 - 占位符1
	glm::vec3 normal;		//4
	float tmpocc2;			//7 - 占位符2
	glm::vec3 color;		//8
	float roughness;		//11
	float metallic;			//12
	float emission;			//13
	float tmpocc3;			//14 - 占位符3
	float tmpocc4;			//15 - 占位符4

	ObjPlane(){
		point = glm::vec3(0.0);
		normal = glm::vec3(0.0, 1.0, 0.0);
		color = glm::vec3(1.0);
		roughness = 0;
		metallic = 0;
		emission = 0;
	}
	ObjPlane(glm::vec3 p, glm::vec3 n, glm::vec3 co, float ro, float me, float em){
		point = p;
		normal = glm::normalize(n);
		color = co;
		roughness = ro;
		metallic = me;
		emission = em;
	}
};							//14N -> 16N in total

//场景类
class Scene{
private:
	Shader rshader;
	Shader averageShader;		//取平均值的shader,默认
	Camera mainCamera;
	glm::mat4 camRotation;
	bool cameraMoved;			//上一次渲染到现在相机是否移动
	bool cameraLocked;			//是否锁定相机视角
	bool worldChanged;			//世界属性是否改变
	
	// scene data ------------------------------------------------------------------
	int sphereCount;
	int planeCount;
	ObjSphere spheres[64];
	ObjPlane planes[64];
	int* const objectCount_ptr[2] = { &sphereCount, &planeCount };
	const char *object_name[2] = { "sphere", "plane" };

	glm::vec3 backgroundColor;

	// OpenGL data -----------------------------------------------------------------
	const long long ByteN = 4;
	GLuint objectUBO;							//场景中所有物体的uniform缓冲
	GLuint sceneFBO, averageFBO;				//帧缓冲,用来累积之前的渲染结果并取平均
	GLuint frameTexture, averageTexture;		//绑定到帧缓冲的纹理
	GLuint rectVAO, rectVBO;					//渲染一个填充整个屏幕的矩形
	int sampleCount;							//帧采样数,每渲染一帧此值+1,移动时清零
	const int maxSampleCount = 1024;
	float screenRectData[24] = 
		{ 1.0, 1.0, 1.0, 1.0,
		 -1.0, -1.0, 0.0, 0.0,
		  1.0, -1.0, 1.0, 0.0,
		 -1.0, -1.0, 0.0, 0.0,
		  1.0, 1.0, 1.0, 1.0,
		 -1.0, 1.0, 0.0, 1.0};

	// ImGUI -----------------------------------------------------------------------
	ImGuiIO *imgui_io;

	// private functions -----------------------------------------------------------
	void ReGenerateTexture(GLuint *texName);
	void ReGenerateTexture(GLuint *texName, GLenum attach);

	void RenderOnce();

public:
	Scene();

	void Init();
	void InitUniformBuffer();
	void InitGUI(GLFWwindow *win);


	void SetScreenSize(int width, int height);
	
	//设置shader
	void SetShader(const char *vertex_file_path, const char *fragment_file_path, std::string name);
	void SetShader(Shader nshd);

	//处理所有输入
	//deltatime为两次调用帧更新函数的间隔时间
	void InputDetect(GLFWwindow *window, float deltatime);


	void AddSphere(ObjSphere newobj);
	void AddPlane(ObjPlane newobj);


	void RenderGUI();
	void Render();
};
