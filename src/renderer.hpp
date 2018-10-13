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
	glm::vec3 waterSize{5000,20,5000};

	glm::vec3 skyColor{ 0.69, 0.84, 1 };
	glm::vec3 sunDir{1,0.5,0};

	ShaderProgram pointShader;
	std::vector<glm::vec3> points;
	GLuint pointVAO;
	GLuint pointVBO;
	

	GLuint skyboxVAO;
	GLuint skyboxVBO;
	double turbidity = 2.2;
	double Yz, xz, yz;
	double zenith;
	double azimuth;
	ShaderProgram skyboxShader;

	float skyCoeffsY[5];
	float skyCoeffsx[5];
	float skyCoeffsy[5];
	
	void calcZenitalAbsolutes();


	GLuint waterNormalTex;

	float globalTime;
	Camera camera;
public:

	void init();
	void render();

	void setGlobalTime(float t)
	{
		globalTime = t;
	}
	void setCamera(const Camera& camera)
	{
		this->camera = camera;
	}

	void drawPoint(glm::vec3 pos)
	{
		points.push_back(pos);
	}
};
