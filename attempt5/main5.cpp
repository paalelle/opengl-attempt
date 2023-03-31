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


//camera: 初始位于(0,0,0),面向z轴负方向

const int modelCount = 5;

//ModelLinker testcube(&testmodel_cube, &simple_shader);
//ModelLinker testsphere(&testmodel_sphere, &simple_shader);
//ModelLinker test1(&testmodel1, &testshader);

//模型位置
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -3.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -3.0f),
	glm::vec3(0.0f, -3.0f, -10.0f),
	glm::vec3(10.0f, -2.0f, -10.0f)
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


	//重新定义视口矩形
	glViewport(0, 0, newWindowWidth, newWindowHeight);
	scene.SetScreenSize(newWindowWidth, newWindowHeight);
}


//初始化
void init(){
	//将光标设置在窗口中央
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	//隐藏光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//======================================OpenGL设置=======================================

	//=======================================================================================


	int curWindowWidth, curWindowHeight;
	glfwGetFramebufferSize(window, &curWindowWidth, &curWindowHeight);

	//设置改变窗口大小时的回调函数
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


//每帧的渲染循环主函数
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
	//初始化GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}


	//声明版本与核心
	//必须兼容OpenGL版本3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//采用OpenGL的核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	window = glfwCreateWindow(windowWidth, windowHeight, "test", NULL, NULL);
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