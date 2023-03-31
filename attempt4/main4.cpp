#include <iostream>
#include <Windows.h>


//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "world.h"



GLFWwindow *window;
bool isMinimized = false;

//=======================================================================================
//��display()����(ÿ֡��Ⱦ����)����ʹ�õı���ȫ������Ϊȫ�ֱ���
//�����ں�������ʱ�����ڴ�,����Ч��
//shader������uniform������λ��(ID)
//�ֱ����mv_matrix, proj_matrix
glm::mat3 normalMat;
Texture tex_wood, tex_concrete;
Model testmodel_cube, testmodel_sphere, testmodel_ground;
Model testmodel1;

Scene scene;
//=======================================================================================


//camera: ��ʼλ��(0,0,0),����z�Ḻ����

const int modelCount = 5;

//ModelLinker testcube(&testmodel_cube, &simple_shader);
//ModelLinker testsphere(&testmodel_sphere, &simple_shader);
//ModelLinker test1(&testmodel1, &testshader);

//ģ��λ��
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -3.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -3.0f),
	glm::vec3(0.0f, -3.0f, -10.0f),
	glm::vec3(10.0f, -2.0f, -10.0f)
};




//�û��������ڴ�Сʱ�Ļص�����
void OnWindowRescale(GLFWwindow *window, int newWindowWidth, int newWindowHeight){
	//std::cout << "width = " << newWindowWidth << " , height = " << newWindowHeight << std::endl;
	//���ô�����С��ʱ��״̬
	if(newWindowWidth == 0 && newWindowHeight == 0){
		isMinimized = true;
		return;
	}
	else isMinimized = false;

	windowAspect = (float)newWindowWidth / newWindowHeight;

	//���¶����ӿھ���
	glViewport(0, 0, newWindowWidth, newWindowHeight);
	//���¶���ͶӰ����
	scene.ResetProjMatrix();
}


//��ʼ��
void init(){

	//����������ڴ�������
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	//���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//======================================OpenGL����=======================================
	//������Ȳ���
	glEnable(GL_DEPTH_TEST);
	//��Ȳ���ʱ���������: <= ʱ������Ȼ���
	glDepthFunc(GL_LEQUAL);
	//�����޳�
	glEnable(GL_CULL_FACE);
	//=======================================================================================


	int curWindowWidth, curWindowHeight;
	glfwGetFramebufferSize(window, &curWindowWidth, &curWindowHeight);
	windowAspect = (float)curWindowWidth / curWindowHeight;

	//���øı䴰�ڴ�Сʱ�Ļص�����
	glfwSetWindowSizeCallback(window, OnWindowRescale);

	//====================================���糡����ʼ��======================================

	scene.Init();

	//--------------------------------------������ɫ��----------------------------------------
	scene.NewShader("shaders/shader_simple.vert", "shaders/shader_simple.frag", "simple_shader");

	//---------------------------------------����ģ��-----------------------------------------
	//scene.NewModel("models/testcube_triangle2.obj", "testmodel_cube");
	//scene.NewModel("models/test_uvsphere_smooth.obj", "testmodel_sphere");

	tex_wood.LoadFromFile("textures/Wood.png", texType_Diffuse);
	tex_concrete.LoadFromFile("textures/Concrete.png", texType_Diffuse);

	testmodel_cube.ImportModel("models/testcube_triangle2.obj");
	testmodel_cube.Rename("testmodel_cube");
	testmodel_cube.AddTexture(tex_wood);
	testmodel_ground.ImportModel("models/testcube_triangle2.obj");
	testmodel_ground.Rename("testmodel_ground");
	testmodel_ground.AddTexture(tex_concrete);
	testmodel_sphere.ImportModel("models/test_uvsphere_smooth.obj");
	testmodel_sphere.Rename("testmodel_sphere");
	testmodel_sphere.AddTexture(tex_concrete);

	scene.NewModel(testmodel_cube);
	scene.NewModel(testmodel_sphere);
	scene.NewModel(testmodel_ground);
	

	//-------------------------------------����ģ��ʵ��---------------------------------------
	ModelLinker *mdlk;

	scene.NewModelInstance("testmodel_cube", "simple_shader");
	mdlk = scene.GetModelLinker(0);
	mdlk->NewModelMat();
	mdlk->Move(0, glm::vec3(-0.5f, -2.0f, -3.0f));
	mdlk->NewModelMat();
	mdlk->Move(1, glm::vec3(2.3f, -2.0f, -5.0f));
	mdlk->NewModelMat();
	mdlk->Move(2, glm::vec3(1.0f, -2.0f, 3.0f));

	scene.NewModelInstance("testmodel_sphere", "simple_shader");
	mdlk = scene.GetModelLinker(1);
	mdlk->NewModelMat();
	mdlk->Move(-1, glm::vec3(-3.0f, -2.0f, -3.0f));

	scene.NewModelInstance("testmodel_ground", "simple_shader");
	mdlk = scene.GetModelLinker(2);
	mdlk->NewModelMat();
	mdlk->Move(-1, glm::vec3(0.0f, -13.0f, -0.0f));
	mdlk->Scale(-1, 10.0);


	//scene.NewModelInstance("ganyu", "anime_shader");
	//mdlk = scene.GetModelLinker(2);
	//mdlk->NewModelMat();
	//mdlk->Move(-1, modelPos[3]);
	//mdlk->Scale(-1, 5.0);


}


//ÿ֡����Ⱦѭ��������
void framedisplay(){
	static double lastFrameTime = glfwGetTime(), curFrameTime;
	curFrameTime = glfwGetTime();

	scene.InputDetect(window, (float)(curFrameTime - lastFrameTime));

	//����clearcolor
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//�����ɫ������(��clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//�����Ȼ�����
	glClear(GL_DEPTH_BUFFER_BIT);


	scene.Render();

	
	lastFrameTime = curFrameTime;
}

int main(){
	//��ʼ��GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}


	//�����汾�����
	//�������OpenGL�汾3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//����OpenGL�ĺ���ģʽ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	window = glfwCreateWindow(windowWidth, windowHeight, "test", NULL, NULL);
	//��������OpenGL��������ϵ����
	//�õ�ǰ���ڵĻ����ڵ�ǰ�߳��ϳ�Ϊ��ǰ����,ʹ���������ڴ˴����Ͻ���
	glfwMakeContextCurrent(window);

	//��ʼ��GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return -1;
	}

	//������glfwSwapBuffers()�����ú󵽻���������ǰ�ȴ�����Ļ������
	//(�򿪴�ֱͬ��)
	glfwSwapInterval(1);


	init();

	while(!glfwWindowShouldClose(window)){

		if(isMinimized){
			Sleep(1);
			continue;
		}

		framedisplay();

		//������ɫ������(�������Ļ��)
		glfwSwapBuffers(window);
		//����������¼�
		glfwPollEvents();
	}

	//���ٴ��ڼ���������
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}