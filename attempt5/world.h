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
	float tmpocc1;			//10 - ռλ��1 (std140������Ҫ����4N)
	float tmpocc2;			//11 - ռλ��2

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
	float tmpocc1;			//1 - ռλ��1
	glm::vec3 normal;		//4
	float tmpocc2;			//7 - ռλ��2
	glm::vec3 color;		//8
	float roughness;		//11
	float metallic;			//12
	float emission;			//13
	float tmpocc3;			//14 - ռλ��3
	float tmpocc4;			//15 - ռλ��4

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

//������
class Scene{
private:
	Shader rshader;
	Shader averageShader;		//ȡƽ��ֵ��shader,Ĭ��
	Camera mainCamera;
	glm::mat4 camRotation;
	bool cameraMoved;			//��һ����Ⱦ����������Ƿ��ƶ�
	bool cameraLocked;			//�Ƿ���������ӽ�
	bool worldChanged;			//���������Ƿ�ı�
	
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
	GLuint objectUBO;							//���������������uniform����
	GLuint sceneFBO, averageFBO;				//֡����,�����ۻ�֮ǰ����Ⱦ�����ȡƽ��
	GLuint frameTexture, averageTexture;		//�󶨵�֡���������
	GLuint rectVAO, rectVBO;					//��Ⱦһ�����������Ļ�ľ���
	int sampleCount;							//֡������,ÿ��Ⱦһ֡��ֵ+1,�ƶ�ʱ����
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
	
	//����shader
	void SetShader(const char *vertex_file_path, const char *fragment_file_path, std::string name);
	void SetShader(Shader nshd);

	//������������
	//deltatimeΪ���ε���֡���º����ļ��ʱ��
	void InputDetect(GLFWwindow *window, float deltatime);


	void AddSphere(ObjSphere newobj);
	void AddPlane(ObjPlane newobj);


	void RenderGUI();
	void Render();
};
