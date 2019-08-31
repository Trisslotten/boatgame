#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"

class Water
{
public:
	void init();
	void update(float globalTime);

	void bindDisplacementTex();
	float getScale();

	int getTexSize();

private: 
	void butterfly(GLuint hTex); 

	int dispIndex = 0;
	GLuint waterDispTexs[2];
	GLuint waterh0Tex;
	GLuint waterhTex;
	GLuint waterhdxTex;
	GLuint waterhdzTex;
	GLuint waterTwiddleTex;
	GLuint waterPing;
	GLuint waterPong;
	ShaderProgram waterTwiddleShader;
	ShaderProgram waterPreFFTShader;
	ShaderProgram waterhShader;
	ShaderProgram waterFFTShader;
	ShaderProgram waterDispShader;
};