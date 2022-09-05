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
//��display()����(ÿ֡��Ⱦ����)����ʹ�õı���ȫ������Ϊȫ�ֱ���
//�����ں�������ʱ�����ڴ�,����Ч��
glm::vec3 cameraPosition;
glm::mat4 projMat, viewMat, modelMat, mvMat;

//shader������uniform������λ��(ID)
//�ֱ����mv_matrix, proj_matrix
GLint mvLoc, projLoc;
//=======================================================================================


//����ģ���е����ж���λ��
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
//ģ��λ��
glm::vec3 modelPos[modelCount] = {
	glm::vec3(0.0f, -2.0f, -5.0f),
	glm::vec3(3.0f, -2.0f, -5.0f),
	glm::vec3(-3.0f, -2.0f, -5.0f)
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
void init(GLFWwindow *window){

	cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
	

	//======================================��ɫ������========================================
	//������Ȳ���
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
	//����vao,��־ÿ��vao��ID(GLuint����)�����ص�vao������
	glGenVertexArrays(numVAO, vao);
	//��("����")��Ӧ��vao,�˺󽫶Դ�vao����
	glBindVertexArray(vao[0]);
	//����vbo,��־ÿ��vbo��ID���ص�vbo������
	glGenBuffers(numVBO, vbo);

	//��("����")��Ӧ��vbo,�˺󽫶Դ�vbo����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//��GPU������(��Ӧ���ļ����vbo��)д������
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertPos), vertPos, GL_STATIC_DRAW);

	//������Ⱦʱ�����������������
	//����ֵΪ0:layout(location = 0)
	//�������Դ�СΪ3(��Ϊ��vec3����,һ������ռ3��������)
	//��������Ϊfloat
	//�������ݱ�׼��,����ϵͳ�Զ�����
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, NULL);
	//���ö�����������
	glEnableVertexAttribArray(0);
}


//ÿ֡����Ⱦѭ��������
void framedisplay(GLFWwindow *window, double curFrameTime, GLuint programId){

	//����clearcolor
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//�����ɫ������(��clearcolor)
	glClear(GL_COLOR_BUFFER_BIT);
	//�����Ȼ�����
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId);
	//=======================================================================================

	//���shader������uniform������λ��(ID)
	mvLoc = glGetUniformLocation(programId, "mv_matrix");
	projLoc = glGetUniformLocation(programId, "proj_matrix");

	viewMat = glm::translate(glm::mat4(1.0f), -cameraPosition);
	for(int i = 0; i < modelCount; i++){
		modelMat = glm::translate(glm::mat4(1.0f), modelPos[i]);
		//modelMat *= glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 1.0f, 0.2f));
		mvMat = viewMat * modelMat;
		//glm::mat4 mvMat = modelMat * viewMat;		����˻�˳��ӦΪ	View * Model

		//��������Ϣ�͸���Ӧ��uniform����
		//value_ptr()���ؾ��������
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}



	
	//=======================================================================================
	
}

int main(){
	GLFWwindow *window;

	//��ʼ��GLFW
	if(!glfwInit()){
		std::cout << "Failed to initialize GLFW" << std::endl;
		system("pause");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	// 
	//�����汾�����
	//�������OpenGL�汾4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	window = glfwCreateWindow(windowWidth, windowHeight, "test_1", NULL, NULL);
	//��������OpenGL��������ϵ����
	//�õ�ǰ���ڵĻ����ڵ�ǰ�߳��ϳ�Ϊ��ǰ����,ʹ���������ڴ˴����Ͻ���
	glfwMakeContextCurrent(window);
	/*
	(The term ��context�� refers to an OpenGL instance and its state information,
	which includes items such as the color buffer.)
	*/

	/*
	//��ʼ��GLEW
	if(glewInit() != GLEW_OK){
		std::cout << "Failed to initialize GLEW" << std::endl;
		system("pause");
		return -1;
	}
	*/
	//��ʼ��GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return -1;
	}

	//������glfwSwapBuffers()�����ú󵽻���������ǰ�ȴ�����Ļ������
	//(�򿪴�ֱͬ��)
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

		//����������
		glfwSwapBuffers(window);
		//����������¼�
		glfwPollEvents();
	}

	//���ٴ��ڼ���������
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}