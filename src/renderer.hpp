#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>
#include "model.hpp"
#include "skybox.hpp"

class Renderer
{
	const int MAX_PATCHES = 128*2.0;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	GLuint waterNormalTex;
	ShaderProgram waterShader;
	glm::vec3 waterSize{5000,20,5000};


	GLuint waterDispTex;
	const int WATER_TEX_SIZE = 512;
	const float WATER_SIZE_SCALE = 100;
	const float WATER_FREQ_SCALE = 100;
	ShaderProgram waterDispShader;

	Skybox skybox;
	
	ShaderProgram modelShader;
	std::unordered_map<std::string, Model> models;

	ShaderProgram voxelShader;

	std::vector<Model*> drawList;

	float globalTime;
	Camera camera;
	glm::mat4 cameraTransform;
public:

	void init();
	void render();

	float getGlobalTime() { return globalTime; }
	void setGlobalTime(float t) { globalTime = t; }
	void setCamera(const Camera& camera) { this->camera = camera; }
	glm::mat4 getCameraTransform() { return cameraTransform; }

	Model* getModel(const std::string& filepath);

	void submit(Model* model);
};
