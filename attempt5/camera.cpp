#include "camera.h"


const int windowWidth = 1024, windowHeight = 768;
const int maxWindowWidth = 1920, maxWindowHeight = 1080;


//初始化:位于(0,0,0),面向z轴负方向
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


//相机前后移动(对应按键w,s)
void Camera::MoveCamera_Forward(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition += frontVector * amount;
}
//相机左右移动(对应按键a,d)
void Camera::MoveCamera_Side(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition += sideVector * amount;
}
//相机垂直移动(对应按键space和Lshift)
void Camera::MoveCamera_Vertical(float delta){
	float amount = cameraMovementSpeed * delta;
	cameraPosition.y += amount;
}
//相机水平转动角度thetaH,垂直转动角度thetaV(均为逆时针)
//(鼠标往左移动=往左转=thetaH增加)
//(鼠标往上移动=往上转=thetaV增加)
void Camera::RotateCamera(float xpos, float ypos){
	float thetaH = cameraRotationSpeed * float(windowWidth / 2 - xpos);
	float thetaV = cameraRotationSpeed * float(windowHeight / 2 - ypos);

	cameraRotationH += thetaH;
	cameraRotationV += thetaV;
	if(cameraRotationV > vAngleLimit) cameraRotationV = vAngleLimit;
	else if(cameraRotationV < -vAngleLimit) cameraRotationV = -vAngleLimit;

	//H:球坐标系下φ(与z轴正向顺时针夹角)		V:球坐标系下θ=π/2-H
	//计算此角度对应的单位向量(y轴向上)
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


