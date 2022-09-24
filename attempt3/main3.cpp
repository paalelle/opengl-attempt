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
//将display()函数(每帧渲染调用)中需使用的变量全部定义为全局变量
//避免在函数中临时分配内存,降低效率
//shader程序中uniform变量的位置(ID)
//分别代表mv_matrix, proj_matrix
glm::mat4 projMat, viewMat, modelMat, mvMat;
glm::mat3 normalMat;
Camera mainCam;
Shader testshader;
Mesh testmesh_cube, testmesh_sphere;
//=======================================================================================


const int modelCount = 4;
MeshLinker testmarray[modelCount];

//模型位置
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -5.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -5.0f),
	glm::vec3(0.0f, -3.0f, 0.0f)
};




//用户调整窗口大小时的回调函数
void OnWindowRescale(GLFWwindow *window, int newWindowWidth, int newWindowHeight){
	//std::cout << "width = " << newWindowWidth << " , height = " << newWindowHeight << std::endl;
	//设置窗口最小化时的状态
	if(newWindowWidth == 0 && newWindowHeight == 0){
		isMinimized = true;
		return;
	}
	else isMinimized = false;

	windowAspect = (float)newWindowWidth / newWindowHeight;

	//重新定义视口矩形
	glViewport(0, 0, newWindowWidth, newWindowHeight);
	//重新定义投影矩阵
	projMat = glm::perspective(FOV, windowAspect, 0.1f, 1000.0f);
}


//初始化
void init(){

	mainCam.SetCamera(glm::vec3(0.0f, 0.0f, 5.0f), PI_f, 0);

	//将光标设置在窗口中央
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	//隐藏光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//======================================着色器设置========================================
	//启用深度测试
	glEnable(GL_DEPTH_TEST);
	//背面剔除
	glEnable(GL_CULL_FACE);
	//?
	glDepthFunc(GL_LEQUAL);
	//======================================定义着色器========================================
	
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

	//设置改变窗口大小时的回调函数
	glfwSetWindowSizeCallback(window, OnWindowRescale);

	//=======================================定义模型=========================================
	
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


//每帧的渲染循环主函数
void framedisplay(){
	static double lastFrameTime = glfwGetTime(), curFrameTime;
	curFrameTime = glfwGetTime();

	InputDetect(window, (float)(curFrameTime - lastFrameTime), mainCam);

	//设置clearcolor
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//清除颜色缓冲区(用clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//清除深度缓冲区
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
	//初始化GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}


	//声明版本与核心
	//必须兼容OpenGL版本3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//采用OpenGL的核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	window = glfwCreateWindow(windowWidth, windowHeight, "test_1", NULL, NULL);
	//将窗口与OpenGL上下文联系起来
	//让当前窗口的环境在当前线程上成为当前环境,使后续操作在此窗口上进行
	glfwMakeContextCurrent(window);

	//初始化GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return -1;
	}

	//设置在glfwSwapBuffers()被调用后到缓冲区交换前等待的屏幕更新数
	//(打开垂直同步)
	glfwSwapInterval(1);


	init();

	while(!glfwWindowShouldClose(window)){

		if(isMinimized){
			Sleep(1);
			continue;
		}

		framedisplay();

		//交换颜色缓冲区(输出在屏幕上)
		glfwSwapBuffers(window);
		//处理窗口相关事件
		glfwPollEvents();
	}

	//销毁窗口及其上下文
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}