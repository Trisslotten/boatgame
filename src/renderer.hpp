#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>
#include "model.hpp"

class Renderer
{
	const int MAX_PATCHES = 128*1.7;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	GLuint waterNormalTex;
	ShaderProgram waterShader;
	glm::vec3 waterSize{5000,20,5000};

	ShaderProgram pointShader;
	std::vector<glm::vec3> points;
	GLuint pointVAO;
	GLuint pointVBO;
	

	GLuint skyboxVAO;
	GLuint skyboxVBO;
	glm::vec3 sunDir;
	glm::vec3 sunColor;
	float turbidity = 2.2f;
	float Yz, xz, yz;
	float zenith;
	float azimuth;
	ShaderProgram skyboxShader;


	float skyCoeffsY[5];
	float skyCoeffsx[5];
	float skyCoeffsy[5];

	void calcSkyValues();
	float skyGamma(float z, float a);


	ShaderProgram modelShader;
	Model model;
	

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
