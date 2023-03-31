#include "shader.h"

Shader::Shader(){
	shaderID = 0;
	isValid = false;
}
Shader::Shader(const char *vert_path, const char *frag_path){
	shaderID = 0;
	isValid = false;
	LoadShader(vert_path, frag_path);
}
Shader::Shader(const char *vert_path, const char *frag_path, std::string name){
	shaderID = 0;
	isValid = false;
	LoadShader(vert_path, frag_path);
	shaderName = name;
}

bool Shader::IsLoaded(){
	return isValid;
}
std::string Shader::GetName(){
	return shaderName;
}
void Shader::Rename(std::string name){
	shaderName = name;
}

void Shader::Use(){
	if(!isValid) return;
	glUseProgram(shaderID);
}

//value_ptr()返回引用,类型为 GLfloat *

void Shader::SendUniform_bool(bool value, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint loc = glGetUniformLocation(shaderID, var_name);
	glUniform1i(loc, (int)value);
}
void Shader::SendUniform_Int(int value, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint loc = glGetUniformLocation(shaderID, var_name);
	glUniform1i(loc, value);
}
void Shader::SendUniform_float(float value, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint loc = glGetUniformLocation(shaderID, var_name);
	glUniform1f(loc, value);
}
void Shader::SendUniform_Vec2(float x, float y, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform2f(vLoc, x, y);
}
void Shader::SendUniform_Vec2(const GLfloat *v, GLsizei count, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform2fv(vLoc, count, v);
}
void Shader::SendUniform_Vec3(float x, float y, float z, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform3f(vLoc, x, y ,z);
}
void Shader::SendUniform_Vec3(const GLfloat *v, GLsizei count, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform3fv(vLoc, count, v);
}
void Shader::SendUniform_Vec4(float x, float y, float z, float w, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform4f(vLoc, x, y, z, w);
}
void Shader::SendUniform_Vec4(const GLfloat *v, GLsizei count, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint vLoc = glGetUniformLocation(shaderID, var_name);

	glUniform4fv(vLoc, count, v);
}
void Shader::SendUniform_Mat3(const GLfloat *m, GLsizei count, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	GLint mLoc = glGetUniformLocation(shaderID, var_name);

	glUniformMatrix3fv(mLoc, count, GL_FALSE, m);
}
void Shader::SendUniform_Mat4(const GLfloat *m, GLsizei count, const char *var_name){
	if(!isValid) return;
	glUseProgram(shaderID);
	//获得shader程序中uniform变量的位置(ID)
	GLint mLoc = glGetUniformLocation(shaderID, var_name);

	//将矩阵信息送给对应的uniform变量
	//value_ptr()返回矩阵的引用
	//第三个参数GL_FALSE代表不对矩阵做转置操作
	glUniformMatrix4fv(mLoc, count, GL_FALSE, m);
}






//检查是否有GLSL编译错误
void printShaderLog(GLuint t_shaderID, std::string shaderName){
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv(t_shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(t_shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if(Result == GL_FALSE){
		std::cout << shaderName << " 编译错误..." << std::endl;
	}
	if(InfoLogLength > 0){
		std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(t_shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		printf("%s\n", &ShaderErrorMessage[0]);
	}
}

//检查是否有GLSL链接错误
void printProgramLog(int t_programID){
	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetProgramiv(t_programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(t_programID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if(Result == GL_FALSE){
		std::cout << "链接失败..." << std::endl;
	}
	if(InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(t_programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
}


void Shader::LoadShader(const char *vertex_file_path, const char *fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else{
		printf("Can not open %s.\n", vertex_file_path);
		isValid = false;
		return;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const *VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	printShaderLog(VertexShaderID, "vertex shader");


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const *FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	printShaderLog(FragmentShaderID, "fragment shader");



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();

	//将着色器附加给program object
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	printProgramLog(ProgramID);

	
	//在把着色器对象链接到程序对象以后就不再需要它们了，删除着色器对象
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	

	isValid = true;
	shaderID = ProgramID;
}


