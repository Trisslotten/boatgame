#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>

class Renderer
{
	const int MAX_PATCHES = 128;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	ShaderProgram waterShader;
	glm::vec3 waterSize{1000,20,1000};

	Camera camera;
	Timer timer;
public:

	void init();
	void render();

	void setCamera(const Camera& camera)
	{
		this->camera = camera;
	}
};