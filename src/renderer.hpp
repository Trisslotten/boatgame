#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>
#include "model.hpp"
#include "skybox.hpp"

class Renderer
{
	const int MAX_PATCHES = 128*1.7;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	GLuint waterNormalTex;
	ShaderProgram waterShader;
	glm::vec3 waterSize{5000,20,5000};

	Skybox skybox;
	
	ShaderProgram modelShader;
	std::unordered_map<std::string, Model> models;

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
};
