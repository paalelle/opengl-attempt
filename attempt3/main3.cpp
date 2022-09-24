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



GLFWwindow *window;
bool isMinimized = false;

//=======================================================================================
//��display()����(ÿ֡��Ⱦ����)����ʹ�õı���ȫ������Ϊȫ�ֱ���
//�����ں�������ʱ�����ڴ�,����Ч��
//shader������uniform������λ��(ID)
//�ֱ����mv_matrix, proj_matrix
glm::mat4 projMat, viewMat, modelMat, mvMat;
glm::mat3 normalMat;
Camera mainCam;
Shader testshader;
Mesh testmesh_cube, testmesh_sphere;
//=======================================================================================


const int modelCount = 4;
MeshLinker testmarray[modelCount];

//ģ��λ��
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -5.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -5.0f),
	glm::vec3(0.0f, -3.0f, 0.0f)
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
	projMat = glm::perspective(FOV, windowAspect, 0.1f, 1000.0f);
}


//��ʼ��
void init(){

	mainCam.SetCamera(glm::vec3(0.0f, 0.0f, 5.0f), PI_f, 0);

	//����������ڴ�������
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	//���ع��
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//======================================��ɫ������========================================
	//������Ȳ���
	glEnable(GL_DEPTH_TEST);
	//�����޳�
	glEnable(GL_CULL_FACE);
	//?
	glDepthFunc(GL_LEQUAL);
	//======================================������ɫ��========================================
	
	testshader.LoadShader("shaders/vert.glsl", "shaders/frag.glsl");
	if(!testshader.IsLoaded()){
		system("pause");
		exit(-1);
	}
	
	//=======================================================================================

	int curWindowWidth, curWindowHeight;
	glfwGetFramebufferSize(window, &curWindowWidth, &curWindowHeight);
	windowAspect = (float)curWindowWidth / curWindowHeight;
	projMat = glm::perspective(FOV, windowAspect, 0.1f, 1000.0f);

	//���øı䴰�ڴ�Сʱ�Ļص�����
	glfwSetWindowSizeCallback(window, OnWindowRescale);

	//=======================================����ģ��=========================================
	
	testmesh_cube.ImportModel("models/testcube_triangle2.obj");
	testmesh_sphere.ImportModel("models/test_uvsphere_smooth.obj");

	for(int i = 0; i < modelCount; i++){
		testmarray[i].Move(modelPos[i]);
	}
	testmarray[0].SetLinkedMesh(&testmesh_cube);
	testmarray[1].SetLinkedMesh(&testmesh_cube);
	testmarray[2].SetLinkedMesh(&testmesh_cube);
	testmarray[3].SetLinkedMesh(&testmesh_sphere);
}


//ÿ֡����Ⱦѭ��������
void framedisplay(){
	static double lastFrameTime = glfwGetTime(), curFrameTime;
	curFrameTime = glfwGetTime();

	InputDetect(window, (float)(curFrameTime - lastFrameTime), mainCam);

	//����clearcolor
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//�����ɫ������(��clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//�����Ȼ�����
	glClear(GL_DEPTH_BUFFER_BIT);

	//testshader.Use();
	
	testshader.SendUniform_Vec3(glm::value_ptr(mainCam.GetPosition()), 1, "viewPos");
	testshader.SendUniform_Vec3(0.5, -2.0, -1.0, "lightDir");
	testshader.SendUniform_Vec3(1.0, 1.0, 1.0, "lightColor");

	//=======================================================================================

	viewMat = mainCam.GetViewMat();
	for(int i = 0; i < modelCount; i++){
		modelMat = testmarray[i].GetModelMat();
		normalMat = glm::mat3(transpose(inverse(modelMat)));
		mvMat = viewMat * modelMat;

		testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
		testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
		testmarray[i].Draw(&testshader);
	}

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