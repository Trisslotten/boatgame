#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>
#include "model.hpp"
#include "skybox.hpp"
#include "water.hpp"

class Renderer
{
public:

	void init();
	void render();


	float getGlobalTime() { return globalTime; }
	void setGlobalTime(float t) { globalTime = t; }
	void setCamera(const Camera& camera) { this->camera = camera; }
	glm::mat4 getCameraTransform() { return cameraTransform; }

	Model* getModel(const std::string& filepath);

	void submit(Model* model);

private:
	const int MAX_PATCHES = 128*2.0;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	GLuint waterNormalTex;
	ShaderProgram waterShader;
	glm::vec3 waterSize{5000,20,5000};

	Water water;

	Skybox skybox;
	
	ShaderProgram modelShader;
	std::unordered_map<std::string, Model> models;

	ShaderProgram voxelShader;

	std::vector<Model*> drawList;

	float globalTime;
	Camera camera;
	glm::mat4 cameraTransform;


	Model* boatModel;
	glm::vec3 boatPos{ 0,1,0 };
	glm::vec3 boatVel;
	glm::vec3 forces;
	glm::vec3 torques;
	glm::vec3 momentum;
	glm::quat orientation;
	glm::vec3 angularMomentum;
	float mass = 40;

	Timer deltaTimer;
};
