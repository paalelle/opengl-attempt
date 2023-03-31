#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Shader{
private:
	GLuint shaderID;
	bool isValid;

	std::string shaderName;
public:
	Shader();
	Shader(const char *vert_path, const char *frag_path);
	Shader(const char *vert_path, const char *frag_path, std::string name);

	bool IsLoaded();
	std::string GetName();
	void Rename(std::string name);

	//���ô���ɫ��
	void Use();

	//����Ϊ���ӦUniform�����������ݵĺ���
	void SendUniform_bool(bool value, const char *var_name);
	void SendUniform_Int(int value, const char *var_name);
	void SendUniform_float(float value, const char *var_name);

	void SendUniform_Vec3(float x, float y, float z, const char *var_name);
	void SendUniform_Vec3(const GLfloat *v, GLsizei count, const char *var_name);
	void SendUniform_Vec4(float x, float y, float z, float w, const char *var_name);
	void SendUniform_Vec4(const GLfloat *v, GLsizei count, const char *var_name);

	void SendUniform_Mat3(const GLfloat *m, GLsizei count, const char *var_name);
	void SendUniform_Mat4(const GLfloat *m, GLsizei count, const char *var_name);


	//void SendMatrices(glm::mat4 modelMat, glm::mat4 viewMat);



	void LoadShader(const char *vertex_file_path, const char *fragment_file_path);
};

