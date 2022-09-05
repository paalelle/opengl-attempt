#include "Model.h"


//debug
void Vertex::PrintData(){
	printf_s("pos: (%.2f, %.2f, %.2f) \tnormal: (%.2f, %.2f, %.2f) \ttex: (%.2f, %.2f)\n",
		Position.x, Position.y, Position.z, Normal.x, Normal.y, Normal.z, TexCoords.x, TexCoords.y);
}



Mesh::Mesh(){
	VAO = VBO = EBO = 0;
	isValid = false;
}

bool Mesh::IsLoaded(){
	return isValid;
}

void Mesh::SetGLInfo(){
	//VAO�洢����������ص������Լ�����������vbo
	//VBO����һϵ�ж�����Ϣ(����)
	//EBO�洢���ƵĶ�����VBO�е�����
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	//�󶨶�Ӧ��vbo,�˺�������GL_ARRAY_BUFFER�ϵĲ��������Դ�vbo����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//��GPU������ (��*��һ������*(GL_ARRAY_BUFFER)�󶨵�vbo����) д������
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//��EBO�󶨵�GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


	//������Ⱦʱ�����������������
	//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml


	glEnableVertexAttribArray(0);
	//����λ������,��Ӧlayout(location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), NULL);

	glEnableVertexAttribArray(1);
	//���㷨��������,��Ӧlayout(location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	//����������������,��Ӧlayout(location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(6 * sizeof(float)));

	//���
	glBindVertexArray(0);

	isValid = true;
}
void Mesh::Load(std::vector<Vertex> vert, std::vector<GLuint> ind){
	vertices = vert;
	indices = ind;

	SetGLInfo();
}

void Mesh::Draw(Shader *dshader){
	if(!isValid) return;

	dshader->Use();
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



void Mesh::ImportModel(const char *path){
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	//for(int i = 0; i < scene->mNumMeshes; i++){
	//	aiMesh *m = scene->mMeshes[i];
	
	//���Խ׶�
	//ֻ�ܵ�һ��mesh,���Ҳ�����������
	aiMesh *m = scene->mMeshes[0];
	std::cout << "��������: " << m->mNumVertices << "   ������: " << m->mNumFaces << std::endl;
	for(unsigned j = 0; j < m->mNumVertices; j++){
		Vertex vert;
		glm::vec3 tmpvec;
		tmpvec.x = m->mVertices[j].x;
		tmpvec.y = m->mVertices[j].y;
		tmpvec.z = m->mVertices[j].z;
		vert.Position = tmpvec;

		tmpvec.x = m->mNormals[j].x;
		tmpvec.y = m->mNormals[j].y;
		tmpvec.z = m->mNormals[j].z;
		vert.Normal = tmpvec;

		vert.TexCoords = glm::vec2(0.0, 0.0);

		vertices.push_back(vert);
	}
	//������������
	for(unsigned j = 0; j < m->mNumFaces; j++){
		aiFace face = m->mFaces[j];
		for(unsigned k = 0; k < face.mNumIndices; k++)
			indices.push_back(face.mIndices[k]);
	}


	//}

	//�������
	/*
	for(auto v : vertices){
		v.PrintData();
	}
	for(size_t i = 0; i < indices.size(); i++){
		if(!(i % 3)) std::cout << "f[" << i / 3 + 1 << "]:  ";
		std::cout << i << " ";
		if(i % 3 == 2) std::cout << std::endl;
	}
	*/

	SetGLInfo();

}



MeshLinker::MeshLinker(){
	pmesh = nullptr;
	modelMatrix = glm::mat4(1.0);
}
MeshLinker::MeshLinker(Mesh *pm){
	pmesh = pm;
	modelMatrix = glm::mat4(1.0);
}

glm::mat4 MeshLinker::GetModelMat(){
	return modelMatrix;
}
void MeshLinker::SetModelMat(glm::mat4 m){
	modelMatrix = m;
}
void MeshLinker::SetLinkedMesh(Mesh *pm){
	pmesh = pm;
}

void MeshLinker::Rotate(float yaw, float pitch, float roll){
	glm::mat4 totrotate =
		glm::rotate(
			glm::rotate(
				glm::rotate(glm::mat4(1.0), yaw, glm::vec3(0.0, 1.0, 0.0)),
				pitch, glm::vec3(0.0, 0.0, 1.0)),
			roll, glm::vec3(1.0, 0.0, 0.0));

	modelMatrix = totrotate * modelMatrix;
}
void MeshLinker::Move(glm::vec3 mvec){
	modelMatrix = glm::translate(modelMatrix, mvec);
}
void MeshLinker::Move(float x, float y, float z){
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
}

void MeshLinker::Draw(Shader *dshader){
	pmesh->Draw(dshader);
}