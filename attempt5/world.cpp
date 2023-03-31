#include "world.h"





Scene::Scene(){
	cameraMoved = cameraLocked = false;
	sampleCount = 0;
	sphereCount = planeCount = 0;
	backgroundColor = glm::vec3(0);
}


void Scene::Init(){
	mainCamera.SetCamera(glm::vec3(0.0f, 0.0f, 0.0f), PI_f, 0);

	//设置充满整个屏幕的矩形=======================================
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(screenRectData), screenRectData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//设置帧缓冲,并附加颜色缓冲和纹理==============================
	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	//生成纹理并绑定
	ReGenerateTexture(&frameTexture, GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &averageFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, averageFBO);
	ReGenerateTexture(&averageTexture, GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//==========================================================

	InitUniformBuffer();

	averageShader = Shader("shaders/shader_screen.vert", "shaders/shader_average.frag", "average");
	averageShader.SendUniform_Int(0, "texAverage");
	averageShader.SendUniform_Int(1, "texNew");
}
void Scene::ReGenerateTexture(GLuint *texId){
	glDeleteTextures(1, texId);
	glGenTextures(1, texId);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mainCamera.curScreenWidth, mainCamera.curScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void Scene::ReGenerateTexture(GLuint *texId, GLenum attach){
	glDeleteTextures(1, texId);
	glGenTextures(1, texId);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mainCamera.curScreenWidth, mainCamera.curScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, *texId, 0);
}
void Scene::InitUniformBuffer(){
	glGenBuffers(1, &objectUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, objectUBO);
	//先分配 uniform 缓冲但不填充
	//object: 若干代表每类物体数量的 int 类型变量 + 各类物体数据(同类上限64)
	glBufferData(GL_UNIFORM_BUFFER, (GLsizeiptr)(4 + 12*64 + 16*64) * ByteN, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, objectUBO);
}
void Scene::InitGUI(GLFWwindow *win){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	imgui_io = &ImGui::GetIO(); (void)imgui_io;
	//imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(win, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}


void Scene::SetScreenSize(int width, int height){
	mainCamera.SetScreenSize(width, height);
	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	//生成纹理并绑定
	ReGenerateTexture(&frameTexture, GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &averageFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, averageFBO);
	ReGenerateTexture(&averageTexture, GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::SetShader(const char *vertex_file_path, const char *fragment_file_path, std::string name){
	Shader nshader(vertex_file_path, fragment_file_path, name);
	rshader = nshader;
}
void Scene::SetShader(Shader nshd){
	rshader = nshd;
}



void Scene::InputDetect(GLFWwindow *window, float deltatime){
	static int lastKeyLocked = GLFW_RELEASE;

	if(!cameraLocked){
		if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			return;
		}
		else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if(xpos != windowWidth / 2 || ypos != windowHeight / 2) cameraMoved = true;

		mainCamera.RotateCamera(xpos, ypos);

		//鼠标指针归位至窗口中央
		glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
			mainCamera.MoveCamera_Forward(deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
			mainCamera.MoveCamera_Forward(-deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			mainCamera.MoveCamera_Side(-deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			mainCamera.MoveCamera_Side(deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
			mainCamera.MoveCamera_Vertical(deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
			mainCamera.MoveCamera_Vertical(-deltatime);
			cameraMoved = true;
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			mainCamera.SpeedBoost(true);
		}

		if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			mainCamera.SpeedBoost(true);
		}
		else mainCamera.SpeedBoost(false);
	}

	int curKeyLocked = glfwGetKey(window, GLFW_KEY_L);
	if(curKeyLocked == GLFW_PRESS && lastKeyLocked == GLFW_RELEASE){
		cameraLocked ^= 1;
		if(cameraLocked) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
	}
	lastKeyLocked = curKeyLocked;

	//这个看情况要不要移到main外面去
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}

}

void Scene::AddSphere(ObjSphere newobj){
	spheres[sphereCount++] = newobj;
}
void Scene::AddPlane(ObjPlane newobj){
	planes[planeCount++] = newobj;
}


void Scene::RenderGUI(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool show_another_window = false;
	static int objType = 0;
	static int inputIndex = 0;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	ImGui::SetNextWindowSize(ImVec2(420, 180));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	{

		ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::ColorEdit3("background color", glm::value_ptr(backgroundColor));
		ImGui::SliderInt("Object Type:", &objType, 0, 1);
		ImGui::SameLine();	ImGui::Text(object_name[objType]);


		ImGui::InputInt("Index", &inputIndex);
		if(ImGui::Button("Show")){
			show_another_window ^= 1;
		}
		if(inputIndex < 0 || inputIndex >= *objectCount_ptr[objType]){
			ImGui::Text("Index out of range!");
		}
		else{
			ImGui::Text("Object found.");
		}

		ImGui::Text("FPS: %.1f ", imgui_io->Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if(show_another_window && (inputIndex >= 0 && inputIndex < *objectCount_ptr[objType]))
	{
		ImGui::Begin("Object Settings", &show_another_window);
		ImGui::Text("index = %d", inputIndex);

		switch(objType){
			case 0:
				worldChanged |= ImGui::InputFloat3("position", glm::value_ptr(spheres[inputIndex].position));
				worldChanged |= ImGui::InputFloat("radius", &spheres[inputIndex].radius);
				worldChanged |= ImGui::ColorEdit3("color", glm::value_ptr(spheres[inputIndex].color));
				worldChanged |= ImGui::SliderFloat("roughness", &spheres[inputIndex].roughness, 0.0f, 1.0f);
				worldChanged |= ImGui::SliderFloat("metallic", &spheres[inputIndex].metallic, 0.0f, 1.0f);
				worldChanged |= ImGui::InputFloat("emission", &spheres[inputIndex].emission);
				break;
			case 1:
				worldChanged |= ImGui::ColorEdit3("color", glm::value_ptr(planes[inputIndex].color));
				worldChanged |= ImGui::SliderFloat("roughness", &planes[inputIndex].roughness, 0.0f, 1.0f);
				worldChanged |= ImGui::SliderFloat("metallic", &planes[inputIndex].metallic, 0.0f, 1.0f);
				worldChanged |= ImGui::InputFloat("emission", &planes[inputIndex].emission);
				break;

			default:
				break;
		}

		if(ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Scene::RenderOnce(){
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	rshader.Use();

	camRotation = mainCamera.GetRotationMatrix();
	rshader.SendUniform_float(glfwGetTime(), "randSeed");
	rshader.SendUniform_float(rand() / 1000.0f, "randSeed2");

	rshader.SendUniform_Vec3(glm::value_ptr(mainCamera.cameraPosition), 1, "cameraPos");
	rshader.SendUniform_float(mainCamera.FOV, "cameraFOV");
	rshader.SendUniform_Mat4(glm::value_ptr(camRotation), 1, "cameraRotationMat");
	rshader.SendUniform_Vec2(mainCamera.curScreenWidth, mainCamera.curScreenHeight, "screenScale");

	rshader.SendUniform_Vec3(glm::value_ptr(backgroundColor), 1, "backgroundColor");

	glBindBuffer(GL_UNIFORM_BUFFER, objectUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, ByteN, &sphereCount);
	glBufferSubData(GL_UNIFORM_BUFFER, ByteN, ByteN, &planeCount);
	glBufferSubData(GL_UNIFORM_BUFFER, 4*ByteN, sphereCount*12*ByteN, spheres);
	glBufferSubData(GL_UNIFORM_BUFFER, (4+64*12)* ByteN, planeCount*16*ByteN, planes);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Scene::Render(){
	//若视角发生移动或世界属性发生改变,清空原来累积的缓冲区并重置帧采样数
	if(cameraMoved || worldChanged){
		sampleCount = 0;
		//glClearTexImage(averageTextureIn, 0, GL_RGB, GL_FLOAT, NULL);
	}
	if(sampleCount < maxSampleCount){
		//第一遍渲染:渲染场景至纹理frameTexture
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		RenderOnce();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//第二遍渲染:利用此帧渲染的frameTexture,
		//计算平均后的图像至averageTexture
		glBindFramebuffer(GL_FRAMEBUFFER, averageFBO);
		averageShader.Use();
		averageShader.SendUniform_float(sampleCount, "sampleCount");
		//激活并绑定纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, averageTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, frameTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glActiveTexture(GL_TEXTURE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		sampleCount++;
		printf("%d\n", sampleCount);
	}
	//第三遍渲染:渲染averageTexture至屏幕
	averageShader.SendUniform_float(-1, "sampleCount");
	//averageShader.SendUniform_float(0, "sampleCount");
	glDrawArrays(GL_TRIANGLES, 0, 6);
	cameraMoved = false;
	worldChanged = false;
}


