#pragma once

#define PI_f 3.14159265f


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

extern const int windowWidth;
extern const int windowHeight;
extern const float FOV;
extern float windowAspect;


class Camera{
private:
	glm::vec3 cameraPosition;
	glm::vec3 facingVector;		//������淽������
	glm::vec3 frontVector;		//ǰ����������(facingVector��xOzƽ����ͶӰ����λ��)
	glm::vec3 sideVector;		//����Ҳ��淽������
	glm::vec3 upVector;			//������Ϸ���������

	float cameraRotationH;		//ˮƽ��ת��
	float cameraRotationV;		//��ֱ��ת��

	//��ֱ��ת�Ǿ���ֵ�����ֵ(����)
	const float vAngleLimit = PI_f / 2;

public:

	float cameraMovementSpeed;	//����ƶ��ٶ�
	float cameraRotationSpeed;	//�����ת�ٶ�


	Camera();

	void SetCamera(glm::vec3 pos, float rotationH, float rotationV);

	void MoveCamera_Forward(float amount);
	void MoveCamera_Side(float amount);
	void MoveCamera_Vertical(float amount);
	void RotateCamera(float thetaH, float thetaV);

	glm::vec3 GetPosition();
	glm::mat4 GetViewMat();


};

void InputDetect(GLFWwindow *window, float deltatime, Camera &cam);