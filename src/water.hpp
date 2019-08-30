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

private: 
	void initTex(GLuint& texture, GLint format, int w, int h);

	void butterfly(GLuint hTex); 

	GLuint waterDispTex;
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