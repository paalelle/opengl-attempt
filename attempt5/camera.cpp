#include "camera.h"


const int windowWidth = 1024, windowHeight = 768;
const int maxWindowWidth = 1920, maxWindowHeight = 1080;


//��ʼ��:λ��(0,0,0),����z�Ḻ����
Camera::Camera(){
	curScreenWidth = windowWidth;
	curScreenHeight = windowHeight;
	windowAspect = (float)windowWidth / windowHeight;
	FOV = PI_f / 3;
	cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	facingVector = glm::vec3(0.0f, 0.0f, -1.0f);
	sideVector = glm::vec3(1.0f, 0.0f, 0.0f);
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(0.0f, 0.0f, -1.0f);

	cameraRotationH = 0;
	cameraRotationV = 0;

	cameraMovementSpeed = 2.0f;
	cameraRotationSpeed = 0.005f;
	speedBase = 2.0f;
	boostFactor = 2.5f;
}

void Camera::SetScreenSize(int width, int height){
	curScreenWidth = width;
	curScreenHeight = height;
	windowAspect = (float)width / height;
}
void Camera::SetFOV(float t_fov){
	FOV = t_fov;
}
void Camera::SetCamera(glm::vec3 pos, float rotationH, float rotationV){
	cameraPosition = pos;
	cameraRotationH = rotationH;
	cameraRotationV = rotationV;

	facingVector = glm::vec3(-sin(cameraRotationH) * cos(cameraRotationV),
		sin(cameraRotationV),
		-cos(cameraRotationH) * cos(cameraRotationV));
	sideVector = glm::vec3(cos(cameraRotationH), 0, -sin(cameraRotationH));
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(-sin(cameraRotationH), 0, -cos(cameraRotationH));
}


//���ǰ���ƶ�(��Ӧ����w,s)
void Camera::MoveCamera_Forward(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition += frontVector * amount;
}
//��������ƶ�(��Ӧ����a,d)
void Camera::MoveCamera_Side(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition += sideVector * amount;
}
//�����ֱ�ƶ�(��Ӧ����space��Lshift)
void Camera::MoveCamera_Vertical(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition.y += amount;
}
//���ˮƽת���Ƕ�thetaH,��ֱת���Ƕ�thetaV(��Ϊ��ʱ��)
//(��������ƶ�=����ת=thetaH����)
//(��������ƶ�=����ת=thetaV����)
void Camera::RotateCamera(float xpos, float ypos){
	float thetaH = cameraRotationSpeed * float(windowWidth / 2 - xpos);
	float thetaV = cameraRotationSpeed * float(windowHeight / 2 - ypos);

	cameraRotationH += thetaH;
	cameraRotationV += thetaV;
	if(cameraRotationV > vAngleLimit) cameraRotationV = vAngleLimit;
	else if(cameraRotationV < -vAngleLimit) cameraRotationV = -vAngleLimit;

	//H:������ϵ�¦�(��z������˳ʱ��н�)		V:������ϵ�¦�=��/2-H
	//����˽Ƕȶ�Ӧ�ĵ�λ����(y������)
	facingVector = glm::vec3(-sin(cameraRotationH) * cos(cameraRotationV),
							 sin(cameraRotationV),
							 -cos(cameraRotationH) * cos(cameraRotationV));
	sideVector = glm::vec3(cos(cameraRotationH), 0, -sin(cameraRotationH));
	upVector = glm::cross(sideVector, facingVector);
	frontVector = glm::vec3(-sin(cameraRotationH), 0, -cos(cameraRotationH));
}
void Camera::SpeedBoost(bool boosting){
	if(boosting) cameraMovementSpeed = speedBase * boostFactor;
	else cameraMovementSpeed = speedBase;
}


glm::vec3 Camera::GetPosition(){
	return cameraPosition;
}
glm::vec3 Camera::GetFacingVector(){
	return facingVector;
}
glm::mat4 Camera::GetRotationMatrix(){
	return glm::rotate(
			glm::rotate(glm::mat4(1.0), cameraRotationH, glm::vec3(0.0, 1.0, 0.0)),
			cameraRotationV, glm::vec3(1.0, 0.0, 0.0));
}


