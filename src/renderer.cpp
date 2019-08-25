#include "renderer.hpp"

#include <iostream>
#include "input.hpp"
#include "window.hpp"
#include "lodepng.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace
{
	const int WATER_TEX_SIZE = 1024;
	const int WATER_FFT_SIZE = 1024;
	const int WATER_FFT_LOG2 = glm::round(glm::log2(double(WATER_FFT_SIZE)));
	const float WATER_SCALE = 100;
	const int COMPUTE_LOCAL_SIZE = 16;
}


void Renderer::init()
{
	glm::vec2 *patchVertices = new glm::vec2[MAX_PATCHES * MAX_PATCHES];

	int counter = 0;
	for (int i = 0; i < MAX_PATCHES; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			patchVertices[counter] = glm::vec2(j, i);
			counter++;
		}
		for (int j = 0; j < i; j++)
		{
			patchVertices[counter] = glm::vec2(i, j);
			counter++;
		}
	}

	glGenVertexArrays(1, &waterPatchVAO);
	glBindVertexArray(waterPatchVAO);
	glGenBuffers(1, &waterPatchVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterPatchVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*MAX_PATCHES*MAX_PATCHES, patchVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);
	delete[] patchVertices;

	waterShader.add("water.vert");
	waterShader.add("water.tesc");
	waterShader.add("water.tese");
	waterShader.add("water.frag");
	waterShader.compile();


	std::vector<unsigned char> image;
	unsigned int width, height;
	unsigned error = lodepng::decode(image, width, height, "water_normal.png");
	if (error != 0)
		std::cout << "ERROR: Could not load water normals\n";

	glGenTextures(1, &waterNormalTex);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);



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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_FFT_SIZE, WATER_FFT_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterhTex);
	glBindTexture(GL_TEXTURE_2D, waterhTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_FFT_SIZE, WATER_FFT_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterPing);
	glBindTexture(GL_TEXTURE_2D, waterPing);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_FFT_SIZE, WATER_FFT_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterPong);
	glBindTexture(GL_TEXTURE_2D, waterPong);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, WATER_FFT_SIZE, WATER_FFT_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &waterTwiddleTex);
	glBindTexture(GL_TEXTURE_2D, waterTwiddleTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WATER_FFT_LOG2, WATER_FFT_SIZE, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
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

	int numGroupsFFT = WATER_FFT_SIZE / COMPUTE_LOCAL_SIZE;
	glBindImageTexture(1, waterh0Tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	waterPreFFTShader.use();
	waterPreFFTShader.uniform("waterScale", WATER_SCALE);
	waterPreFFTShader.uniform("fftSize", float(WATER_FFT_SIZE));
	glDispatchCompute(numGroupsFFT, numGroupsFFT, 1);



	int numGroupsTwiddle = WATER_FFT_SIZE / COMPUTE_LOCAL_SIZE;
	glBindImageTexture(3, waterTwiddleTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	waterTwiddleShader.use();
	waterPreFFTShader.uniform("fftSize", float(WATER_FFT_SIZE));
	glDispatchCompute(WATER_FFT_LOG2, WATER_FFT_SIZE/16.0, 1);

	
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	std::vector<glm::vec4> test;
	test.resize(WATER_FFT_LOG2*WATER_TEX_SIZE);
	glGetTextureImage(waterTwiddleTex, 0, GL_RGBA, GL_FLOAT, test.size() * sizeof(glm::vec4), test.data());
	for (int i = 0; i < WATER_FFT_LOG2*WATER_TEX_SIZE; i+= WATER_FFT_LOG2)
	{
		std::cout << test[i].z << ",\t" << test[i].w << "\n";
		//std::cout << test[i].x << ", " << test[i].y << ", " << test[i].z << ", " << test[i].w << "\n";
	}
	

	modelShader.add("model.vert");
	modelShader.add("model.frag");
	modelShader.compile();

	voxelShader.add("voxel.vert");
	voxelShader.add("voxel.geom");
	voxelShader.add("voxel.frag");
	voxelShader.compile();

	skybox.init();
	skybox.update();
}

void Renderer::render()
{
	if (Input::isKeyDown(GLFW_KEY_F5))
	{
		waterShader.reload();
	}
	int numGroupsFFT = WATER_FFT_SIZE / COMPUTE_LOCAL_SIZE;
	int numGroupsWater = WATER_TEX_SIZE / COMPUTE_LOCAL_SIZE;

	glBindImageTexture(1, waterh0Tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(2, waterhTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	waterhShader.use();
	waterhShader.uniform("waterScale", WATER_SCALE);
	waterhShader.uniform("fftSize", float(WATER_FFT_SIZE));
	waterhShader.uniform("time", globalTime);
	glDispatchCompute(numGroupsFFT, numGroupsFFT, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	// horizontal pass
	readPing = true;
	glBindImageTexture(2, waterTwiddleTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	waterFFTShader.use();
	waterFFTShader.uniform("isHorizontalPass", 1);
	for (int i = 0; i < WATER_FFT_LOG2; i++)
	{
		if (readPing)
		{
			if (i == 0)
				glBindImageTexture(0, waterhTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			else
				glBindImageTexture(0, waterPing, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPong, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		else
		{
			glBindImageTexture(0, waterPong, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
			glBindImageTexture(1, waterPing, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
		}
		waterFFTShader.uniform("stage", i);

		glDispatchCompute(numGroupsFFT, numGroupsFFT, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		readPing = !readPing;
	}

	// vertical pass
	waterFFTShader.uniform("isHorizontalPass", 0);
	for (int i = 0; i < WATER_FFT_LOG2; i++)
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
		waterFFTShader.uniform("stage", i);

		glDispatchCompute(numGroupsFFT, numGroupsFFT, 1);

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		readPing = !readPing;
	}


	glBindImageTexture(1, waterDispTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	if(readPing)
		glBindImageTexture(2, waterPing, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	else
		glBindImageTexture(2, waterPong, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	waterDispShader.use();
	waterDispShader.uniform("waterScale", WATER_SCALE);
	waterDispShader.uniform("fftSize", float(WATER_FFT_SIZE));
	waterDispShader.uniform("waterSize", float(WATER_TEX_SIZE));
	glDispatchCompute(numGroupsWater, numGroupsWater, 1);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	cameraTransform = camera.getTransform();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	auto s = Window::size();
	glViewport(0, 0, s.x, s.y);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	modelShader.use();
	modelShader.uniform("viewProj", cameraTransform);
	//modelShader.uniform("sunDir", sunDir);
	modelShader.uniform("time", globalTime);
	for(auto m : drawList)
	{
		//m->render(modelShader);
	}
	
	voxelShader.use();
	voxelShader.uniform("viewProj", cameraTransform);
	voxelShader.uniform("fov", camera.fov);
	for (auto m : drawList)
	{
		glCullFace(GL_FRONT);
		m->renderVoxels(modelShader);
		glCullFace(GL_BACK);
	}
	drawList.clear();


	// render water
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, waterDispTex);
	waterShader.use();
	waterShader.uniform("normalMap", 0);
	waterShader.uniform("dispTex", 1);
	waterShader.uniform("numPatches", float(MAX_PATCHES));
	waterShader.uniform("size", waterSize);
	waterShader.uniform("viewProj", cameraTransform);
	waterShader.uniform("cameraPos", camera.position);
	waterShader.uniform("time", globalTime);
	waterShader.uniform("windowSize", Window::size());
	waterShader.uniform("fov", camera.fov);
	waterShader.uniform("cameraDir", camera.getLookDir());
	waterShader.uniform("sunDir", skybox.getSunDir());
	waterShader.uniform("waterScale", WATER_SCALE);
	skybox.setUniforms(waterShader);
	glBindVertexArray(waterPatchVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glDrawArrays(GL_PATCHES, 0, MAX_PATCHES * MAX_PATCHES);


	skybox.draw(this);
}


Model* Renderer::getModel(const std::string& filepath)
{
	auto elem = models.find(filepath);

	Model* result = nullptr;

	if (elem != models.end())
	{ 
		result = &elem->second;
	}
	else
	{
		auto created = &models[filepath];
		if (created->load(filepath))
			result = created;
		else
			models.erase(filepath);
	}
	return result;
}

void Renderer::submit(Model * model)
{
	drawList.push_back(model);
}
