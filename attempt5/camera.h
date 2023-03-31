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
	glm::vec3 facingVector;		//������淽������
	glm::vec3 frontVector;		//ǰ����������(facingVector��xOzƽ����ͶӰ����λ��)
	glm::vec3 sideVector;		//����Ҳ��淽������
	glm::vec3 upVector;			//������Ϸ���������

	float cameraRotationH;		//ˮƽ��ת��(��ʱ�����,��-z����ļн�)
	float cameraRotationV;		//��ֱ��ת��(��xOzƽ��ļн�,����+y����нǵ����)

	//��ֱ��ת�Ǿ���ֵ�����ֵ(����)
	const float vAngleLimit = PI_f / 2;

	float cameraMovementSpeed;	//����ƶ��ٶ�
	float cameraRotationSpeed;	//�����ת�ٶ�

	float speedBase;			//�����ٶ�
	float boostFactor;			//����ϵ��
	
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

