#include "camera.h"

const int windowWidth = 1024, windowHeight = 768;
const float FOV = PI_f / 3;
float windowAspect;



//初始化:位于(0,0,0),面向z轴负方向
Camera::Camera(){
	cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	facingVector = glm::vec3(0.0f, 0.0f, -1.0f);
	sideVector = glm::vec3(1.0f, 0.0f, 0.0f);
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(0.0f, 0.0f, -1.0f);

	cameraRotationH = 0;
	cameraRotationV = 0;

	cameraMovementSpeed = 2.0f;
	cameraRotationSpeed = 0.005f;
}

void Camera::SetCamera(glm::vec3 pos, float rotationH, float rotationV){
	cameraPosition = pos;
	cameraRotationH = rotationH;
	cameraRotationV = rotationV;

	facingVector = glm::vec3(sin(cameraRotationH) * cos(cameraRotationV),
		sin(cameraRotationV),
		cos(cameraRotationH) * cos(cameraRotationV));
	sideVector = glm::vec3(-cos(cameraRotationH), 0, sin(cameraRotationH));
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(sin(cameraRotationH), 0, cos(cameraRotationH));
}


//相机前后移动(对应按键w,s)
void Camera::MoveCamera_Forward(float amount){
	cameraPosition += frontVector * amount;
}
//相机左右移动(对应按键a,d)
void Camera::MoveCamera_Side(float amount){
	cameraPosition += sideVector * amount;
}
//相机垂直移动(对应按键space和Lshift)
void Camera::MoveCamera_Vertical(float amount){
	cameraPosition.y += amount;
}
//相机水平转动角度thetaH,垂直转动角度thetaV(均为逆时针)
void Camera::RotateCamera(float thetaH, float thetaV){
	cameraRotationH += thetaH;
	cameraRotationV += thetaV;
	if(cameraRotationV > vAngleLimit) cameraRotationV = vAngleLimit;
	else if(cameraRotationV < -vAngleLimit) cameraRotationV = -vAngleLimit;

	//H:球坐标系下φ(与z轴正向顺时针夹角)		V:球坐标系下θ=π/2-H
	//计算此角度对应的单位向量(y轴向上)
	facingVector = glm::vec3(sin(cameraRotationH) * cos(cameraRotationV),
							 sin(cameraRotationV),
							 cos(cameraRotationH) * cos(cameraRotationV));
	sideVector = glm::vec3(-cos(cameraRotationH), 0, sin(cameraRotationH));
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(sin(cameraRotationH), 0, cos(cameraRotationH));
}


glm::vec3 Camera::GetPosition(){
	return cameraPosition;
}
glm::mat4 Camera::GetViewMat(){
	return glm::lookAt(cameraPosition, cameraPosition + facingVector, upVector);
}



//deltatime为两次调用帧更新函数的间隔时间
void InputDetect(GLFWwindow *window, float deltatime, Camera &cam){

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	cam.RotateCamera(cam.cameraRotationSpeed * float(windowWidth / 2 - xpos),
				 cam.cameraRotationSpeed * float(windowHeight / 2 - ypos));

	//鼠标指针归位至窗口中央
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		cam.MoveCamera_Forward(cam.cameraMovementSpeed * deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		cam.MoveCamera_Forward(-cam.cameraMovementSpeed * deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		cam.MoveCamera_Side(-cam.cameraMovementSpeed * deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		cam.MoveCamera_Side(cam.cameraMovementSpeed * deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		cam.MoveCamera_Vertical(cam.cameraMovementSpeed * deltatime);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		cam.MoveCamera_Vertical(-cam.cameraMovementSpeed * deltatime);
	}

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

