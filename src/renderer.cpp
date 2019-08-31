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


	water.init();

	modelShader.add("model.vert");
	modelShader.add("model.frag");
	modelShader.compile();

	voxelShader.add("voxel.vert");
	voxelShader.add("voxel.geom");
	voxelShader.add("voxel.frag");
	voxelShader.compile();

	skybox.init();
	skybox.update();

	////////////////////////////////////

	/*
	buoyancyShader.add("buoyancy.comp");
	buoyancyShader.compile();

	glGenBuffers(1, &boatSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, boatSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 4096, &boatPos, GL_DYNAMIC_DRAW);
	*/

	boatModel = this->getModel("assets/hull.obj");
}

void Renderer::render()
{
	if (Input::isKeyDown(GLFW_KEY_F5))
	{
		waterShader.reload();
	}
	water.update(globalTime);


	float dt = deltaTimer.restart();
	water.bindDisplacementTex();
	std::vector<glm::vec4> dispTex(water.getTexSize()*water.getTexSize());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, dispTex.data());

	int vr = Model::VOXEL_RES;
	for (int z = 0; z < vr; z++)
	{
		for (int y = 0; y < vr; y++)
		{
			for (int x = 0; x < vr; x++)
			{
				int index = x + y * vr + z * vr * vr;
				if (boatModel->img[index] == 0)
					continue;

				glm::vec3 pos = boatPos + glm::vec3(x, y, z) / float(vr) - 0.5f;

				float ws = water.getScale();
				int ix = water.getTexSize()*glm::fract(pos.x / ws);
				int iy = water.getTexSize()*glm::fract(pos.x / ws);
				int dispIndex = ix + iy * water.getTexSize();

				glm::vec4 disp = dispTex[dispIndex];
				if (pos.y < disp.y)
				{
					float volume = 1.f / (vr*vr*vr);
					float displaced = 1000 * volume;
					boatForces.y += 9.82 * displaced;
					float v = length(boatVel);
					if (v > 0.0001f)
						boatForces -= 0.5f * 1000.f * normalize(boatVel) * v*v / float(vr*vr);
				}
			}
		}
	}
	float mass = 50;
	boatForces.y -= 9.82*mass;
	
	boatVel += boatForces * dt / mass;
	boatPos += boatVel * dt;

	boatForces = glm::vec3(0);

	this->submit(boatModel);



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
	modelShader.uniform("position", boatPos);
	for(auto m : drawList)
	{
		m->render(modelShader);
	}
	
	voxelShader.use();
	voxelShader.uniform("viewProj", cameraTransform);
	voxelShader.uniform("fov", camera.fov);
	voxelShader.uniform("position", boatPos);
	for (auto m : drawList)
	{
		glCullFace(GL_FRONT);
		//m->renderVoxels(modelShader);
		glCullFace(GL_BACK);
	}
	drawList.clear();

	// render water
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	glActiveTexture(GL_TEXTURE0+1);
	water.bindDisplacementTex();
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
	waterShader.uniform("waterScale", water.getScale());
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
