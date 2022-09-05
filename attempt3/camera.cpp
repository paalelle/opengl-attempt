#include "camera.h"

const int windowWidth = 1024, windowHeight = 768;
const float FOV = PI_f / 3;
float windowAspect;



//��ʼ��:λ��(0,0,0),����z�Ḻ����
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


//���ǰ���ƶ�(��Ӧ����w,s)
void Camera::MoveCamera_Forward(float amount){
	cameraPosition += frontVector * amount;
}
//��������ƶ�(��Ӧ����a,d)
void Camera::MoveCamera_Side(float amount){
	cameraPosition += sideVector * amount;
}
//�����ֱ�ƶ�(��Ӧ����space��Lshift)
void Camera::MoveCamera_Vertical(float amount){
	cameraPosition.y += amount;
}
//���ˮƽת���Ƕ�thetaH,��ֱת���Ƕ�thetaV(��Ϊ��ʱ��)
void Camera::RotateCamera(float thetaH, float thetaV){
	cameraRotationH += thetaH;
	cameraRotationV += thetaV;
	if(cameraRotationV > vAngleLimit) cameraRotationV = vAngleLimit;
	else if(cameraRotationV < -vAngleLimit) cameraRotationV = -vAngleLimit;

	//H:������ϵ�¦�(��z������˳ʱ��н�)		V:������ϵ�¦�=��/2-H
	//����˽Ƕȶ�Ӧ�ĵ�λ����(y������)
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



//deltatimeΪ���ε���֡���º����ļ��ʱ��
void InputDetect(GLFWwindow *window, float deltatime, Camera &cam){

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	cam.RotateCamera(cam.cameraRotationSpeed * float(windowWidth / 2 - xpos),
				 cam.cameraRotationSpeed * float(windowHeight / 2 - ypos));

	//���ָ���λ����������
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

