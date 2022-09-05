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

const GLuint numVAO = 1, numVBO = 1;

bool isMinimized = false;

GLuint vao[numVAO];
GLuint vbo[numVBO];


float cubeVertex[108] = {
-1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
};

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
	//glEnable(GL_CULL_FACE);
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

	//=======================================================================================
	/*
	//创建vao,标志每个vao的ID(GLuint类型)将返回到vao数组里
	//vao存储顶点属性相关的设置以及与它关联的vbo
	glGenVertexArrays(numVAO, vao);
	//绑定("激活")对应的vao,此后将对此vao操作
	glBindVertexArray(vao[0]);
	//创建vbo,标志每个vbo的ID返回到vbo数组里
	//vbo储存一系列顶点信息(数据)
	glGenBuffers(numVBO, vbo);

	//绑定("激活")对应的vbo,此后所有在GL_ARRAY_BUFFER上的操作都将对此vbo进行
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//向GPU缓冲区 (与*第一个参数*(GL_ARRAY_BUFFER)绑定的vbo对象) 写入数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos), vertPos, GL_STATIC_DRAW);

	//设置渲染时顶点属性数组的数据
	//索引值为0:与VAO[0]绑定,对应layout(location = 0)
	//顶点属性大小为3(因为是vec3类型,一个数据占3个浮点数)
	//数据类型为float
	//不把数据标准化
	//步长设置为3(下个组位置数据在3个float之后)
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
	//启用顶点属性数组
	glEnableVertexAttribArray(0);
	*/

	
	testmesh_cube.ImportModel("models/testcube_triangle.obj");
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
	//viewMat = glm::translate(glm::mat4(1.0f), -cameraPosition);
	/*for(int i = 0; i < modelCount; i++){
		modelMat = glm::translate(glm::mat4(1.0f), modelPos[i]);
		normalMat = glm::mat3(transpose(inverse(modelMat)));
		//modelMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 1.0f, 0.2f));
		mvMat = viewMat * modelMat;
		//glm::mat4 mvMat = modelMat * viewMat;		矩阵乘积顺序应为	View * Model

		//将矩阵信息送给对应的uniform变量
		//value_ptr()返回矩阵的引用
		testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
		testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
		testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
		testmesh_cube.Draw(&testshader);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -3.0, 0.0));
	normalMat = glm::mat3(transpose(inverse(modelMat)));
	mvMat = viewMat * modelMat;
	testshader.SendUniform_Mat4(glm::value_ptr(modelMat), 1, "model_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(mvMat), 1, "mv_matrix");
	testshader.SendUniform_Mat4(glm::value_ptr(projMat), 1, "proj_matrix");
	testshader.SendUniform_Mat3(glm::value_ptr(normalMat), 1, "normal_matrix");
	testmesh_sphere.Draw(&testshader);
	*/
	
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

	//glfwWindowHint(GLFW_SAMPLES, 4);
	// 
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
	/*
	(The term “context” refers to an OpenGL instance and its state information,
	which includes items such as the color buffer.)
	*/

	/*
	//初始化GLEW
	if(glewInit() != GLEW_OK){
		std::cout << "Failed to initialize GLEW" << std::endl;
		system("pause");
		return -1;
	}
	*/

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