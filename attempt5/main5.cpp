#include <iostream>
#include <Windows.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "camera.h"
#include "world.h"


GLFWwindow *window;
bool isMinimized = false;


Scene scene;
Shader shader;
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


	//���¶����ӿھ���
	glViewport(0, 0, newWindowWidth, newWindowHeight);
	scene.SetScreenSize(newWindowWidth, newWindowHeight);
}


//��ʼ��
void init(){
	//����������ڴ�������
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	//���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//======================================OpenGL����=======================================

	//=======================================================================================


	int curWindowWidth, curWindowHeight;
	glfwGetFramebufferSize(window, &curWindowWidth, &curWindowHeight);

	//���øı䴰�ڴ�Сʱ�Ļص�����
	glfwSetWindowSizeCallback(window, OnWindowRescale);

	scene.SetScreenSize(curWindowWidth, curWindowHeight);
	scene.SetShader("shaders/shader_simple.vert", "shaders/shader_pathtracing.frag", "raytracing");
	scene.Init();
	scene.InitGUI(window);

	ObjSphere nsphere1(glm::vec3(0.0, 0.0, 3.0), 0.3, glm::vec3(1.0, 1.0, 1.0), 0.5, 0.5, 20.0);
	ObjSphere nsphere2(glm::vec3(0.0, 0.0, 6.0), 1.0, glm::vec3(0.3, 0.3, 0.7), 1.0, 0.1, 0.2);
	ObjSphere nsphere3(glm::vec3(0.0, 100.0, 0.0), 20.0, glm::vec3(0.0, 1.0, 1.0), 0.0, 0.0, 5.0);
	scene.AddSphere(nsphere1);
	scene.AddSphere(nsphere2);
	scene.AddSphere(nsphere3);
	ObjPlane nplane1(glm::vec3(0.0, -2.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.3, 0.3, 0.3), 0.5, 0.0, 0.0);
	ObjPlane nplane2(glm::vec3(-5.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.4, 0.4), 1.0, 1.0, 0.0);
	ObjPlane nplane3(glm::vec3(5.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.4, 0.4, 1.0), 1.0, 1.0, 0.0);
	ObjPlane nplane4(glm::vec3(0.0, 0.0, 10.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 1.0, 1.0), 1.0, 1.0, 0.0);
	scene.AddPlane(nplane1);
	scene.AddPlane(nplane2);
	scene.AddPlane(nplane3);
	scene.AddPlane(nplane4);

}


//ÿ֡����Ⱦѭ��������
void framedisplay(){
	static double lastFrameTime = glfwGetTime(), curFrameTime;
	curFrameTime = glfwGetTime();

	scene.InputDetect(window, (float)(curFrameTime - lastFrameTime));


	scene.Render();
	scene.RenderGUI();

	
	lastFrameTime = curFrameTime;
}

int main(){
	srand((unsigned)time(0));
	//��ʼ��GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}


	//�����汾�����
	//�������OpenGL�汾3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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