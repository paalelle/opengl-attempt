#include "model.h"




Model::Model(){
	isValid = false;
}
Model::Model(std::string path){
	isValid = false;
	ImportModel(path);
}
Model::Model(std::string path, std::string name){
	isValid = false;
	ImportModel(path);
	modelName = name;
}


void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	// 处理节点所有的网格（如果有的话）
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// 接下来对它的子节点重复这一过程
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}
Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene){
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//处理此mesh上所有顶点信息
	//遍历顶点
	for(unsigned j = 0; j < mesh->mNumVertices; j++){
		Vertex vert;
		glm::vec3 tmpvec;

		//填充顶点坐标信息
		tmpvec.x = mesh->mVertices[j].x;
		tmpvec.y = mesh->mVertices[j].y;
		tmpvec.z = mesh->mVertices[j].z;
		vert.Position = tmpvec;

		//填充顶点法线信息
		if(mesh->HasNormals()){
			tmpvec.x = mesh->mNormals[j].x;
			tmpvec.y = mesh->mNormals[j].y;
			tmpvec.z = mesh->mNormals[j].z;
			vert.Normal = tmpvec;
		}

		//填充顶点纹理坐标信息,只考虑第一组纹理坐标,多了用不到
		if(mesh->mTextureCoords[0]){
			vert.TexCoords.x = mesh->mTextureCoords[0][j].x;
			vert.TexCoords.y = mesh->mTextureCoords[0][j].y;
		}
		else vert.TexCoords = glm::vec2(0.0, 0.0);		//没有纹理坐标


		vertices.push_back(vert);
	}

	//处理顶点索引
	for(unsigned j = 0; j < mesh->mNumFaces; j++){
		aiFace face = mesh->mFaces[j];
		for(unsigned k = 0; k < face.mNumIndices; k++)
			indices.push_back(face.mIndices[k]);
	}
	
	//处理,加载此mesh上所有材质
	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, texType_Diffuse);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> normalMaps = LoadTextures(material, aiTextureType_HEIGHT, texType_Normal);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, texType_Specular);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}


	//测试输出
	/*if(vertices.size() < 100){
		for(auto v : vertices){
			v.PrintData();
		}
		for(size_t i = 0; i < indices.size(); i++){
			if(!(i % 3)) std::cout << "f[" << i / 3 + 1 << "]:  ";
			std::cout << i << " ";
			if(i % 3 == 2) std::cout << std::endl;
		}
	}*/
	std::cout << "model[" << this << "].mesh: texture数: " << textures.size() << std::endl;
	for(unsigned i = 0; i<textures.size(); i++){
		std::cout << "textureType[" << i << "]: " << textures[i].type;
		std::cout << "     id: " << textures[i].id << std::endl;
	}

	return Mesh(vertices, indices, textures);
}
std::vector<Texture> Model::LoadTextures(aiMaterial *aimat, aiTextureType aitype, TextureType ttype)
{
	std::vector<Texture> textures;
	for(unsigned int i = 0; i < aimat->GetTextureCount(aitype); i++)
	{
		aiString pathstr;
		aimat->GetTexture(aitype, i, &pathstr);

		//std::cout << "path: " << pathstr.C_Str() << std::endl;
		 
		Texture tmptex(directory + "/" + pathstr.C_Str(), ttype);
		//tmptex.path = pathstr;
		textures.push_back(tmptex);
	}
	return textures;
}


bool Model::IsLoaded(){
	return isValid;
}
std::string Model::GetName(){
	return modelName;
}
void Model::Rename(std::string name){
	modelName = name;
}

void Model::Draw(Shader *dshader){
	if(dshader == nullptr){
		std::cout << "Warning: Model::Draw被调用但没有指定shader" << std::endl;
		return;
	}

	for(auto mi : meshes){
		mi.Draw(dshader);
	}
}


void Model::ImportModel(std::string path){
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	if(scene->mRootNode == nullptr){
		std::cout << "ERROR:没有mRootNode" << importer.GetErrorString() << std::endl;
		return;
	}

	//模型文件地址
	directory = path.substr(0, path.find_last_of('/'));


	ProcessNode(scene->mRootNode, scene);
	isValid = true;


	//调试输出
	std::cout << "model[" << this << "] mesh数: " << meshes.size() << std::endl;
	/*
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
	*/
}



ModelLinker::ModelLinker(){
	pmodel = nullptr;
	mshader = nullptr;
}
ModelLinker::ModelLinker(Model *pm){
	pmodel = pm;
	mshader = nullptr;
}
ModelLinker::ModelLinker(Model *pm, Shader *ps){
	pmodel = pm;
	mshader = ps;
}


void ModelLinker::SetLinkedModel(Model *pm){
	pmodel = pm;
}
void ModelLinker::BindShader(Shader *dshader){
	mshader = dshader;
}


glm::mat4 ModelLinker::GetModelMat(unsigned index){
	if(index >= modelMatrices.size()){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return glm::mat4(1.0);
	}
	return modelMatrices[index];
}


void ModelLinker::NewModelMat(){
	modelMatrices.push_back(glm::mat4(1.0));
}
void ModelLinker::NewModelMat(glm::mat4 m){
	modelMatrices.push_back(m);
}

void ModelLinker::SetModelMat(int index, glm::mat4 m){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}
	else if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = m;
		}
		return;
	}

	modelMatrices[index] = m;
}
void ModelLinker::Rotate(int index, float yaw, float pitch, float roll){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}

	glm::mat4 totrotate =
		glm::rotate(
			glm::rotate(
				glm::rotate(glm::mat4(1.0), yaw, glm::vec3(0.0, 1.0, 0.0)),
				pitch, glm::vec3(0.0, 0.0, 1.0)),
			roll, glm::vec3(1.0, 0.0, 0.0));

	if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = totrotate * modelMatrices[i];
		}
		return;
	}

	modelMatrices[index] = totrotate * modelMatrices[index];
}
void ModelLinker::Move(int index, glm::vec3 mvec){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}
	else if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = glm::translate(modelMatrices[i], mvec);
		}
		return;
	}

	modelMatrices[index] = glm::translate(modelMatrices[index], mvec);
}
void ModelLinker::Move(int index, float x, float y, float z){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}
	else if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = glm::translate(modelMatrices[i], glm::vec3(x, y, z));
		}
		return;
	}

	modelMatrices[index] = glm::translate(modelMatrices[index], glm::vec3(x, y, z));
}
void ModelLinker::Scale(int index, float val){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}
	else if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = glm::scale(modelMatrices[i], glm::vec3(val, val, val));
		}
		return;
	}

	modelMatrices[index] = glm::scale(modelMatrices[index], glm::vec3(val, val, val));
}
void ModelLinker::Scale(int index, float x, float y, float z){
	if(index >= (int)modelMatrices.size() || index < -1){
		std::cout << "Warning: 模型矩阵下标超出范围,当前模型矩阵数: " << modelMatrices.size() << std::endl;
		return;
	}
	else if(index == -1){
		for(unsigned i = 0; i < modelMatrices.size(); i++){
			modelMatrices[i] = glm::scale(modelMatrices[i], glm::vec3(x, y, z));
		}
		return;
	}

	modelMatrices[index] = glm::scale(modelMatrices[index], glm::vec3(x, y, z));
}


void ModelLinker::DrawAll(Shader *dshader, glm::mat4 mview, glm::mat4 mproj){
	dshader->Use();

	dshader->SendUniform_Mat4(glm::value_ptr(mproj), 1, "proj_matrix");
	for(unsigned i = 0; i < modelMatrices.size(); i++){

		normalMatrix = glm::mat3(transpose(inverse(modelMatrices[i])));
		mvMatrix = mview * modelMatrices[i];
		
		dshader->SendUniform_Mat4(glm::value_ptr(modelMatrices[i]), 1, "model_matrix");
		dshader->SendUniform_Mat4(glm::value_ptr(mvMatrix), 1, "mv_matrix");
		dshader->SendUniform_Mat3(glm::value_ptr(normalMatrix), 1, "normal_matrix");

		pmodel->Draw(dshader);
	}
}
void ModelLinker::DrawAll(glm::mat4 mview, glm::mat4 mproj){
	mshader->Use();

	mshader->SendUniform_Mat4(glm::value_ptr(mproj), 1, "proj_matrix");
	for(unsigned i = 0; i < modelMatrices.size(); i++){

		normalMatrix = glm::mat3(transpose(inverse(modelMatrices[i])));
		mvMatrix = mview * modelMatrices[i];

		mshader->SendUniform_Mat4(glm::value_ptr(modelMatrices[i]), 1, "model_matrix");
		mshader->SendUniform_Mat4(glm::value_ptr(mvMatrix), 1, "mv_matrix");
		mshader->SendUniform_Mat3(glm::value_ptr(normalMatrix), 1, "normal_matrix");

		pmodel->Draw(mshader);
	}
}

