#include "water.hpp"

namespace
{
	const int WATER_TEX_SIZE = 512;
	const int WATER_TEX_LOG2 = glm::round(glm::log2(double(WATER_TEX_SIZE)));
	const float WATER_SCALE = 128;
	const int COMPUTE_LOCAL_SIZE = 16;


	const int numGroups = WATER_TEX_SIZE / COMPUTE_LOCAL_SIZE;
}

void Water::init()
{
	glGenTextures(1, &waterDispTex);
	glBindTexture(GL_TEXTURE_2D, waterDispTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &waterh0Tex);
	glBindTexture(GL_TEXTURE_2D, waterh0Tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterhTex);
	glBindTexture(GL_TEXTURE_2D, waterhTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterhdxTex);
	glBindTexture(GL_TEXTURE_2D, waterhdxTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterhdzTex);
	glBindTexture(GL_TEXTURE_2D, waterhdzTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterPing);
	glBindTexture(GL_TEXTURE_2D, waterPing);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterPong);
	glBindTexture(GL_TEXTURE_2D, waterPong);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_TEX_SIZE, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterTwiddleTex);
	glBindTexture(GL_TEXTURE_2D, waterTwiddleTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WATER_TEX_LOG2, WATER_TEX_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	waterTwiddleShader.add("twiddle.comp");
	waterTwiddleShader.compile();

	waterPreFFTShader.add("waterprefft.comp");
	waterPreFFTShader.compile();

	waterhShader.add("waterh.comp");
	waterhShader.compile();

	waterFFTShader.add("waterfft.comp");
	waterFFTShader.compile();

	waterDispShader.add("water.comp");
	waterDispShader.compile();

	int numGroupsFFT = WATER_TEX_SIZE / COMPUTE_LOCAL_SIZE;
	glBindImageTexture(1, waterh0Tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	waterPreFFTShader.use();
	waterPreFFTShader.uniform("waterScale", WATER_SCALE);
	waterPreFFTShader.uniform("fftSize", float(WATER_TEX_SIZE));
	glDispatchCompute(numGroupsFFT, numGroupsFFT, 1);


	int numGroupsTwiddle = WATER_TEX_SIZE / COMPUTE_LOCAL_SIZE;
	glBindImageTexture(3, waterTwiddleTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	waterTwiddleShader.use();
	waterTwiddleShader.uniform("fftSize", float(WATER_TEX_SIZE));
	glDispatchCompute(WATER_TEX_LOG2, WATER_TEX_SIZE / 16.0, 1);
}

void Water::update(float globalTime)
{
	glBindImageTexture(0, waterh0Tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, waterhTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(2, waterhdxTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(3, waterhdzTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	waterhShader.use();
	waterhShader.uniform("waterScale", WATER_SCALE);
	waterhShader.uniform("fftSize", float(WATER_TEX_SIZE));
	waterhShader.uniform("time", 2*globalTime);
	glDispatchCompute(numGroups, numGroups, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	butterfly(waterhTex);
	butterfly(waterhdxTex);
	butterfly(waterhdzTex);


	glBindImageTexture(0, waterDispTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, waterhTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, waterhdxTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(3, waterhdzTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	waterDispShader.use();
	waterDispShader.uniform("waterScale", WATER_SCALE);
	waterDispShader.uniform("fftSize", float(WATER_TEX_SIZE));
	waterDispShader.uniform("waterSize", float(WATER_TEX_SIZE));
	glDispatchCompute(numGroups, numGroups, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Water::bindDisplacementTex()
{
	glBindTexture(GL_TEXTURE_2D, waterDispTex);
}

float Water::getScale()
{
	return WATER_SCALE;
}

void Water::butterfly(GLuint hTex)
{
	// horizontal pass
	bool readPing = true;
	glBindImageTexture(2, waterTwiddleTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	waterFFTShader.use();
	waterFFTShader.uniform("isHorizontalPass", 1);
	for (int i = 0; i < WATER_TEX_LOG2; i++)
	{
		if (readPing)
		{
			glBindImageTexture(0, waterPing, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPong, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		else
		{
			glBindImageTexture(0, waterPong, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPing, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		if (i == 0)
			glBindImageTexture(0, hTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		waterFFTShader.uniform("stage", i);
		glDispatchCompute(numGroups, numGroups, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		readPing = !readPing;
	}
	// vertical pass
	waterFFTShader.uniform("isHorizontalPass", 0);
	for (int i = 0; i < WATER_TEX_LOG2; i++)
	{
		if (readPing)
		{
			glBindImageTexture(0, waterPing, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPong, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		else
		{
			glBindImageTexture(0, waterPong, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPing, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		if (i == WATER_TEX_LOG2 - 1)
			glBindImageTexture(1, hTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		waterFFTShader.uniform("stage", i);
		glDispatchCompute(numGroups, numGroups, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		readPing = !readPing;
	}
}
