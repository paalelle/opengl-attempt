#pragma once

#define PI_f 3.14159265f


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>



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

	float cameraMovementSpeed;	//����ƶ��ٶ�
	float cameraRotationSpeed;	//�����ת�ٶ�

	float speedBase;			//�����ٶ�
	float boostFactor;			//����ϵ��
	
public:

	Camera();

	void SetCamera(glm::vec3 pos, float rotationH, float rotationV);

	void MoveCamera_Forward(float amount);
	void MoveCamera_Side(float amount);
	void MoveCamera_Vertical(float amount);
	void RotateCamera(float thetaH, float thetaV);
	void SpeedBoost(bool boosting);

	glm::vec3 GetPosition();
	glm::mat4 GetViewMat();


};

