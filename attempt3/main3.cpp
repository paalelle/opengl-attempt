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
Model testmodel_cube, testmodel_sphere, testmodel_nanosuit;
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
	glm::vec3(0.0f, -2.0f, -5.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -5.0f),
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
	scene.NewShader("shaders/vert_simple.glsl", "shaders/frag_simple.glsl", "simple_shader");
	scene.NewShader("shaders/vert.glsl", "shaders/frag.glsl", "testshader");

	//---------------------------------------����ģ��-----------------------------------------
	scene.NewModel("models/testcube_triangle2.obj", "testmodel_cube");
	scene.NewModel("models/test_uvsphere_smooth.obj", "testmodel_sphere");
	scene.NewModel("models/nanosuit/nanosuit.obj", "testmodel_nanosuit");
	scene.NewModel("models/ganyu/ganyu.obj", "ganyu");

	//-------------------------------------����ģ��ʵ��---------------------------------------
	ModelLinker *mdlk;

	scene.NewModelInstance("testmodel_cube", "simple_shader");
	mdlk = scene.GetModelLinker(0);
	mdlk->NewModelMat();
	mdlk->Move(-1, modelPos[0]);

	scene.NewModelInstance("testmodel_sphere", "simple_shader");
	mdlk = scene.GetModelLinker(1);
	mdlk->NewModelMat();
	mdlk->Move(-1, modelPos[2]);

	scene.NewModelInstance("ganyu", "testshader");
	mdlk = scene.GetModelLinker(2);
	mdlk->NewModelMat();
	mdlk->Move(-1, modelPos[3]);
	mdlk->Scale(-1, 5.0);


}


//ÿ֡����Ⱦѭ��������
void framedisplay(){
	static double lastFrameTime = glfwGetTime(), curFrameTime;
	curFrameTime = glfwGetTime();

	scene.InputDetect(window, (float)(curFrameTime - lastFrameTime));

	//����clearcolor
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//�����ɫ������(��clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//�����Ȼ�����
	glClear(GL_DEPTH_BUFFER_BIT);


	scene.Render();

	
	/*
	simple_shader.SendUniform_Vec3(glm::value_ptr(mainCam.GetPosition()), 1, "viewPos");
	simple_shader.SendUniform_Vec3(0.5, -2.0, -1.0, "lightDir");
	simple_shader.SendUniform_Vec3(1.0, 1.0, 1.0, "lightColor");

	testshader.SendUniform_Vec3(glm::value_ptr(mainCam.GetPosition()), 1, "viewPos");
	testshader.SendUniform_Vec3(0.5, -2.0, -1.0, "lightDir");
	testshader.SendUniform_Vec3(1.0, 1.0, 1.0, "lightColor");
	*/

	//=======================================================================================

	//viewMat = mainCam.GetViewMat();

	//testcube.DrawAll(viewMat, projMat);
	//testsphere.DrawAll(viewMat, projMat);
	//test1.DrawAll(viewMat, projMat);

	/*modelMat = testcube.GetModelMat(0);
	normalMat = glm::mat3(transpose(inverse(modelMat)));
	mvMat = viewMat * modelMat;
	simple_shader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
	simple_shader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
	simple_shader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
	simple_shader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
	testmodel_cube.Draw(&simple_shader);


	modelMat = testsphere.GetModelMat(0);
	normalMat = glm::mat3(transpose(inverse(modelMat)));
	mvMat = viewMat * modelMat;
	simple_shader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
	simple_shader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
	simple_shader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
	simple_shader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
	testmodel_sphere.Draw(&simple_shader);*/
	

	/*modelMat = testmodel_nanosuit.GetModelMat();
	normalMat = glm::mat3(transpose(inverse(modelMat)));
	mvMat = viewMat * modelMat;
	testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
	testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
	testmodel_nanosuit.Draw(&testshader);*/


	/*modelMat = testmodel1.GetModelMat();
	normalMat = glm::mat3(transpose(inverse(modelMat)));
	mvMat = viewMat * modelMat;
	testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
	testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
	testmodel1.Draw(&testshader);*/

	/*for(int i = 0; i < modelCount; i++){
		modelMat = testmarray[i].GetModelMat();
		normalMat = glm::mat3(transpose(inverse(modelMat)));
		mvMat = viewMat * modelMat;

		testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
		testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
		testmarray[i].Draw(&testshader);
	}*/

	//=======================================================================================
	
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
	window = glfwCreateWindow(windowWidth, windowHeight, "test_1", NULL, NULL);
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