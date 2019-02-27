#pragma once

#include "shader.hpp"

class Renderer;

class Skybox
{
	GLuint skyboxVAO;
	GLuint skyboxVBO;
	glm::vec3 sunDir;
	glm::vec3 sunColor;
	float turbidity = 2.1f;
	float Yz, xz, yz;
	float zenith;
	float azimuth;
	ShaderProgram skyboxShader;

	float skyCoeffsY[5];
	float skyCoeffsx[5];
	float skyCoeffsy[5];

	float skyGamma(float z, float a);
public:

	void init();

	void update();

	void setUniforms(ShaderProgram& shader);

	void draw(Renderer* renderer);


	glm::vec3 getSunDir() { return sunDir; }
};