#include "mesh.h"


//debug
void Vertex::PrintData(){
	printf_s("pos: (%.2f, %.2f, %.2f) \tnormal: (%.2f, %.2f, %.2f) \ttex: (%.2f, %.2f)\n",
		Position.x, Position.y, Position.z, Normal.x, Normal.y, Normal.z, TexCoords.x, TexCoords.y);
}



Texture::Texture(){
	id = 0;
	type = texType_None;
	isValid = false;
}
Texture::Texture(std::string path, TextureType t){
	id = 0;
	type = texType_None;
	isValid = false;
	LoadFromFile(path, t);
}


void Texture::LoadFromFile(std::string path, TextureType t){
	// 加载并生成纹理
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if(data)
	{
		type = t;

		glGenTextures(1, &id);

		GLenum format = GL_RGB;
		//按照读入的图像信息选择颜色通道
		if(nrChannels == 1)
			format = GL_RED;
		else if(nrChannels == 3)
			format = GL_RGB;
		else if(nrChannels == 4)
			format = GL_RGBA;
		

		//绑定此纹理
		glBindTexture(GL_TEXTURE_2D, id);

		//生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);						//设置s轴(x轴)的环绕模式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);						//设置t轴(y轴)的环绕模式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		//缩小时纹理采样使用线性过滤模式(在相邻mipmap采样)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);					//放大时纹理采样使用线性过滤模式

		//解绑
		glBindTexture(GL_TEXTURE_2D, 0);

		isValid = true;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}






Mesh::Mesh(){
	VAO = VBO = EBO = 0;
	isValid = false;
}
Mesh::Mesh(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture> tex){
	Load(vert, ind, tex);
	SetGLInfo();
}

void Mesh::AddTexture(Texture tex){
	textures.push_back(tex);
}


bool Mesh::IsLoaded(){
	return isValid;
}

void Mesh::SetGLInfo(){
	//VAO存储顶点属性相关的设置以及与它关联的vbo
	//VBO储存一系列顶点信息(数据)
	//EBO存储绘制的顶点在VBO中的索引
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	//绑定对应的vbo,此后所有在GL_ARRAY_BUFFER上的操作都将对此vbo进行
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//向GPU缓冲区 (与*第一个参数*(GL_ARRAY_BUFFER)绑定的vbo对象) 写入数据
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//把EBO绑定到GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


	//设置渲染时顶点属性数组的数据
	//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml


	glEnableVertexAttribArray(0);
	//顶点位置数据,对应layout(location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), NULL);

	glEnableVertexAttribArray(1);
	//顶点法向量数据,对应layout(location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	//顶点纹理坐标数据,对应layout(location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(6 * sizeof(float)));

	//解绑定
	glBindVertexArray(0);

	isValid = true;
}
void Mesh::Load(std::vector<Vertex> vert, std::vector<GLuint> ind, std::vector<Texture> tex){
	vertices = vert;
	indices = ind;
	textures = tex;

	SetGLInfo();
}


void Mesh::Draw(Shader *dshader){
	if(!isValid){
		std::cout << "Warning: Mesh::Draw被调用但这Mesh还没初始化" << std::endl;
		return;
	}
	if(dshader == nullptr){
		std::cout << "Warning: Mesh::Draw被调用但没有指定shader" << std::endl;
		return;
	}

	dshader->Use();

	unsigned diffuseNr = 0;
	unsigned normalNr = 0;
	unsigned specularNr = 0;
	for(unsigned i = 0; i < textures.size(); i++){
		TextureType ttype = textures[i].type;
		std::string texNumber;
		std::string texTypeName;

		//纹理未加载,跳过
		if(ttype == texType_None) continue;

		if(ttype == texType_Diffuse){
			texNumber = std::to_string(diffuseNr++);
			texTypeName = "diffuse";
		}
		else if(ttype == texType_Normal){
			texNumber = std::to_string(normalNr++);
			texTypeName = "normal";
		}
		else if(ttype == texType_Specular){
			texNumber = std::to_string(specularNr++);
			texTypeName = "specular";
		}


		//std::cout << "texture" + texTypeName + texNumber << "     --     texture unit " << i;
		//std::cout << "     id: " << textures[i].id << std::endl;
		//命名规则:texture[纹理序号].纹理类别
		//设置着色器中每个sampler对应的纹理单元
		dshader->SendUniform_Int(i, ("textures[" + texNumber + "]." + texTypeName).c_str());

		//在绑定之前激活相应的纹理单元
		glActiveTexture(GL_TEXTURE0 + i);

		//绑定对应纹理
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	dshader->Use();
	//绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//切换回默认纹理0
	glActiveTexture(GL_TEXTURE0);
}



void Mesh::ImportMesh(const char *path){
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	//for(int i = 0; i < scene->mNumMeshes; i++){
	//	aiMesh *m = scene->mMeshes[i];

		//测试阶段
		//只管第一个mesh
	aiMesh *m = scene->mMeshes[0];
	std::cout << "顶点总数: " << m->mNumVertices << "   面总数: " << m->mNumFaces << std::endl;
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

		//只考虑第一组纹理坐标
		if(m->mTextureCoords[0]){
			vert.TexCoords.x = m->mTextureCoords[0][j].x;
			vert.TexCoords.y = m->mTextureCoords[0][j].y;
		}
		else vert.TexCoords = glm::vec2(0.0, 0.0);


		vertices.push_back(vert);
	}
	//处理顶点索引
	for(unsigned j = 0; j < m->mNumFaces; j++){
		aiFace face = m->mFaces[j];
		for(unsigned k = 0; k < face.mNumIndices; k++)
			indices.push_back(face.mIndices[k]);
	}


	//}

	//调试输出
	if(vertices.size() < 100){
		for(auto v : vertices){
			v.PrintData();
		}
		for(size_t i = 0; i < indices.size(); i++){
			if(!(i % 3)) std::cout << "f[" << i / 3 + 1 << "]:  ";
			std::cout << i << " ";
			if(i % 3 == 2) std::cout << std::endl;
		}
	}


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
	modelMatrix = glm::translate(modelMatrix, mvec) * modelMatrix;
}
void MeshLinker::Move(float x, float y, float z){
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z)) * modelMatrix;
}

void MeshLinker::Draw(Shader *dshader){
	pmesh->Draw(dshader);
}
