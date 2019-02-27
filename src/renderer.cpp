#include "renderer.hpp"

#include <iostream>
#include "input.hpp"
#include "window.hpp"
#include "lodepng.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>




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


	modelShader.add("model.vert");
	modelShader.add("model.frag");
	modelShader.compile();

	skybox.init();
	skybox.update();
}

void Renderer::render()
{
	if (Input::isKeyDown(GLFW_KEY_F5))
	{
		waterShader.reload();
	}
	cameraTransform = camera.getTransform();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	auto s = Window::size();
	glViewport(0, 0, s.x, s.y);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	modelShader.use();
	modelShader.uniform("viewProj", cameraTransform);
	modelShader.uniform("sunDir", sunDir);
	modelShader.uniform("time", globalTime);
	model.render(modelShader);
	*/

	// render water
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	waterShader.use();
	waterShader.uniform("numPatches", float(MAX_PATCHES));
	waterShader.uniform("normalMap", 0);
	waterShader.uniform("size", waterSize);
	waterShader.uniform("viewProj", cameraTransform);
	waterShader.uniform("cameraPos", camera.position);
	waterShader.uniform("time", globalTime);
	waterShader.uniform("windowSize", Window::size());
	waterShader.uniform("fov", camera.fov);
	waterShader.uniform("cameraDir", camera.getLookDir());
	waterShader.uniform("sunDir", skybox.getSunDir());
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