#include <iostream>
#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderloader.h"

const GLuint numVAO = 1, numVBO = 1;
const int modelCount = 3;
const int windowWidth = 1024, windowHeight = 768;
const float FOV = 3.14159f / 3;

float windowAspect;
bool isMinimized = false;

GLuint vao[numVAO];
GLuint vbo[numVBO];


//=======================================================================================
//将display()函数(每帧渲染调用)中需使用的变量全部定义为全局变量
//避免在函数中临时分配内存,降低效率
glm::vec3 cameraPosition;
glm::mat4 projMat, viewMat, modelMat, mvMat;

//shader程序中uniform变量的位置(ID)
//分别代表mv_matrix, proj_matrix
GLint mvLoc, projLoc;
//=======================================================================================


//单个模型中的所有顶点位置
float vertPos[] = { 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };

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
//模型位置
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -5.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -5.0f)
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
void init(GLFWwindow *window){

	cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	

	//======================================着色器设置========================================
	//启用深度测试
	glEnable(GL_DEPTH_TEST);
	//?
	glDepthFunc(GL_LEQUAL);
	//=======================================================================================

	int curWindowWidth, curWindowHeight;
	glfwGetFramebufferSize(window, &curWindowWidth, &curWindowHeight);
	windowAspect = (float)curWindowWidth / curWindowHeight;
	projMat = glm::perspective(FOV, windowAspect, 0.1f, 1000.0f);

	glfwSetWindowSizeCallback(window, OnWindowRescale);

	//=======================================================================================
	//创建vao,标志每个vao的ID(GLuint类型)将返回到vao数组里
	glGenVertexArrays(numVAO, vao);
	//绑定("激活")对应的vao,此后将对此vao操作
	glBindVertexArray(vao[0]);
	//创建vbo,标志每个vbo的ID返回到vbo数组里
	glGenBuffers(numVBO, vbo);

	//绑定("激活")对应的vbo,此后将对此vbo操作
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//向GPU缓冲区(对应上文激活的vbo中)写入数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos), vertPos, GL_STATIC_DRAW);

	//设置渲染时顶点属性数组的数据
	//索引值为0:layout(location = 0)
	//顶点属性大小为3(因为是vec3类型,一个数据占3个浮点数)
	//数据类型为float
	//不把数据标准化,步长系统自动设置
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);
	//启用顶点属性数组
	glEnableVertexAttribArray(0);
}


//每帧的渲染循环主函数
void framedisplay(GLFWwindow *window, double curFrameTime, GLuint programId){

	//设置clearcolor
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//清除颜色缓冲区(用clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//清除深度缓冲区
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId);
	//=======================================================================================

	//获得shader程序中uniform变量的位置(ID)
	mvLoc = glGetUniformLocation(programId, "mv_matrix");
	projLoc = glGetUniformLocation(programId, "proj_matrix");

	viewMat = glm::translate(glm::mat4(1.0f), -cameraPosition);
	for(int i = 0; i < modelCount; i++){
		modelMat = glm::translate(glm::mat4(1.0f), modelPos[i]);
		//modelMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 1.0f, 0.2f));
		mvMat = viewMat * modelMat;
		//glm::mat4 mvMat = modelMat * viewMat;		矩阵乘积顺序应为	View * Model

		//将矩阵信息送给对应的uniform变量
		//value_ptr()返回矩阵的引用
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}



	
	//=======================================================================================
	
}

int main(){
	GLFWwindow *window;

	//初始化GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	// 
	//声明版本与核心
	//必须兼容OpenGL版本4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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


	GLuint programID = LoadShaders("shaders/vert.glsl", "shaders/frag.glsl");
	if(!programID){
		system("pause");
		return -1;
	}



	init(window);

	while(!glfwWindowShouldClose(window)){

		if(isMinimized){
			Sleep(1);
			continue;
		}

		framedisplay(window, glfwGetTime(), programID);

		//交换缓冲区
		glfwSwapBuffers(window);
		//处理窗口相关事件
		glfwPollEvents();
	}

	//销毁窗口及其上下文
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}