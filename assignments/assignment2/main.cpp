#include <stdio.h>
#include <math.h>
#include<ew/procGen.h>
#include <ew/external/glad.h>
#include<ew/shader.h>
#include<ew/model.h>
#include<ew/camera.h>
#include<ew/transform.h>
#include<ew/cameraController.h>
#include<ew/texture.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include<bstone/shadowmapfb.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
ew::Transform monkeyTransform;
ew::CameraController cameraController;
ew::Camera camera;
ew::Camera shadowCam;
ew::Transform planeTransform;
ew::Mesh planeMesh;
ben::Framebuffer shadowfb;

//Global state
int screenWidth = 1080;
int screenHeight = 720;
float prevFrameTime;
float deltaTime;
struct Bias
{
	float minBias;
	float maxBias;
}bias;
struct Light
{
	glm::vec3 dir = glm::vec3(1, 0, 0); 
	glm::vec3 col;
}light;
struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;
struct ColorIntensity
{
	float rScale = 1.0;
	float gScale = 1.0;
	float bScale = 1.0;
}colormod;


int main() {

	GLFWwindow* window = initWindow("Assignment 2", screenWidth, screenHeight);
	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");
	ben::Framebuffer fb = ben::createFramebuffer(screenWidth, screenHeight, GL_RGB16F);
	shadowfb = ben::createShadowFramebuffer(screenHeight, screenHeight, GL_DEPTH_COMPONENT32);
	ew::Shader shadowShader = ew::Shader("assets/shadow.vert", "assets/shadow.frag");
	ew::Shader postProcess = ew::Shader("assets/bstonevert.vert", "assets/bstonefrag.frag");
	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Model monkeyModel = ew::Model("assets/Suzanne.obj");
	planeMesh = ew::Mesh(ew::createPlane(10, 10, 5));
	planeTransform.position = glm::vec3(0, -3, 0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing
	
	#pragma region camera
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees
	#pragma endregion Camera

	#pragma region shadowCam
	shadowCam.orthographic = true;
	shadowCam.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	shadowCam.nearPlane = 0.01;
	shadowCam.farPlane = 20;
	shadowCam.orthoHeight = 5;
	shadowCam.aspectRatio = 1;
	//glm::mat4 viewmatrix = shadowCam.viewMatrix();
	//glm::mat4 projectionmatrix = shadowCam.projectionMatrix();
#pragma endregion shadowCam
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	//Initialization…
	unsigned int dummyVAO;
	glCreateVertexArrays(1, &dummyVAO);
	
	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;
		shadowCam.position = shadowCam.target - light.dir * 5.0f;//negative light direction
		#pragma region Shadow
		glBindFramebuffer(GL_FRAMEBUFFER, shadowfb.fbo);
		glViewport(0, 0, shadowfb.width, shadowfb.height);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		shadowShader.use();
		shadowShader.setMat4("_ViewProjection", shadowCam.projectionMatrix() * shadowCam.viewMatrix());
		shadowShader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw();
		//plane
		shader.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();
		#pragma endregion Shadow
		
		#pragma region Lighting
		glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
		glViewport(0, 0, fb.width, fb.height);
		glClearColor(0.6f,0.8f,0.92f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTextureUnit(0, brickTexture);
		glBindTextureUnit(1, shadowfb.depthBuffer);
		
		shader.use();
		shader.setFloat("minBias", bias.minBias);
		shader.setFloat("maxBias", bias.maxBias);

		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);

		shader.setInt("_ShadowMap", 1);

		shader.setVec3("_EyePos", camera.position);
		//shader.setMat4("_Model", glm::mat4(1.0f));
		shader.setVec3("_LightDirection", light.dir);
		shader.setMat4("_LightViewProjection", shadowCam.projectionMatrix()* shadowCam.viewMatrix());
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		

		shader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw(); //Draws monkey model using current shader
		#pragma endregion 
		//plane
		shader.setMat4("_Model", planeTransform.modelMatrix());
		planeMesh.draw();
		cameraController.move(window, &camera, deltaTime);
		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		
		

		//Post Process (currently Grayscale)
		
		postProcess.use();
		glBindTextureUnit(0, fb.colorBuffer[0]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, screenWidth, screenHeight);



		glBindVertexArray(dummyVAO);
		//6 vertices for quad, 3 for triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
		drawUI();


		

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}
	if (ImGui::CollapsingHeader("Light"))
	{
		ImGui::SliderFloat3("LightDirection",&light.dir.x,-1.0,1.0);
	}
	if (ImGui::CollapsingHeader("Bias"))
	{
		ImGui::SliderFloat("minBias", &bias.minBias, 0.0, 0.005);
		ImGui::SliderFloat("maxBias", &bias.maxBias, 0.015, 0.03);
	}

	ImGui::End(); 
	ImGui::Begin("Shadow Map");
	//Using a Child allow to fill all the space of the window.
	ImGui::BeginChild("Shadow Map");
	//Stretch image to be window size
	ImVec2 windowSize = ImGui::GetWindowSize();
	//Invert 0-1 V to flip vertically for ImGui display
	//shadowMap is the texture2D handle
	ImGui::Image((ImTextureID)shadowfb.depthBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
/// 
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

