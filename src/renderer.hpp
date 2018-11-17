#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "timer.hpp"
#include <GLFW/glfw3.h>

class Renderer
{
	const int MAX_PATCHES = 2*128;

	GLuint waterPatchVAO;
	GLuint waterPatchVBO;
	GLuint waterNormalTex;
	// renders wave dispacement in texture
	ShaderProgram waveShader;
	// uses displacement texture to render water
	ShaderProgram waterShader;
	glm::vec3 waterSize{5000,20,5000};
	const GLsizei WAVE_RES = 1024;
	const float WAVE_TILE_SIZE = 10.f;
	GLuint waveFramebuffer;
	GLuint waveTexture;

	GLuint quadVAO;
	GLuint quadVBO;

	ShaderProgram pointShader;
	std::vector<glm::vec3> points;
	GLuint pointVAO;
	GLuint pointVBO;

	GLuint skyboxVAO;
	GLuint skyboxVBO;
	glm::vec3 sunDir;
	glm::vec3 sunColor;
	float turbidity = 2.2;
	float Yz, xz, yz;
	float zenith;
	float azimuth;
	ShaderProgram skyboxShader;

	GLuint boatVAO;
	GLuint boatVBO;
	ShaderProgram boatShader;

	

	float skyCoeffsY[5];
	float skyCoeffsx[5];
	float skyCoeffsy[5];

	void calcSkyValues();
	float skyGamma(float z, float a);

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
