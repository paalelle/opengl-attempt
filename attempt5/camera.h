#pragma once

#define PI_f 3.14159265f


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>


extern const int windowWidth;
extern const int windowHeight;
extern const int maxWindowWidth;
extern const int maxWindowHeight;



class Camera{
friend class Scene;
private:
	int curScreenWidth, curScreenHeight;
	float windowAspect;
	float FOV;

	glm::vec3 cameraPosition;
	glm::vec3 facingVector;		//相机正面方向向量
	glm::vec3 frontVector;		//前进方向向量(facingVector在xOz平面上投影并单位化)
	glm::vec3 sideVector;		//相机右侧面方向向量
	glm::vec3 upVector;			//相机正上方方向向量

	float cameraRotationH;		//水平旋转角(逆时针计算,与-z方向的夹角)
	float cameraRotationV;		//垂直旋转角(与xOz平面的夹角,即与+y方向夹角的余角)

	//垂直旋转角绝对值的最大值(弧度)
	const float vAngleLimit = PI_f / 2;

	float cameraMovementSpeed;	//相机移动速度
	float cameraRotationSpeed;	//相机旋转速度

	float speedBase;			//基础速度
	float boostFactor;			//加速系数
	
public:

	Camera();

	void SetScreenSize(int width, int height);
	void SetFOV(float t_fov);
	void SetCamera(glm::vec3 pos, float rotationH, float rotationV);

	void MoveCamera_Forward(float amount);
	void MoveCamera_Side(float amount);
	void MoveCamera_Vertical(float amount);
	void RotateCamera(float thetaH, float thetaV);
	void SpeedBoost(bool boosting);

	glm::vec3 GetPosition();
	glm::vec3 GetFacingVector();

	glm::mat4 GetRotationMatrix();
};

