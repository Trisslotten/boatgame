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

	boatModel = this->getModel("assets/hull2.obj");
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

				glm::mat3 rotation = glm::mat3_cast(orientation);

				glm::vec3 offset = rotation * (glm::vec3(x, y, z) / float(vr) - 0.5f);
				glm::vec3 pos = boatPos + offset;

				int ts = water.getTexSize();
				float ws = water.getScale();
				int ix = int(ts*glm::fract(pos.x / ws)) % ts;
				int iy = int(ts*glm::fract(pos.z / ws)) % ts;
				int dispIndex = ix + iy * water.getTexSize();

				

				glm::vec4 disp = dispTex[dispIndex];
				if (pos.y < disp.y)
				{

					float volume = 1.f / (vr*vr*vr);
					float displaced = 1000.f * volume;

					glm::vec3 force(0, 9.82f * displaced, 0);

					torques -= glm::cross(force, offset);
					forces += force;

					glm::vec3 velAt = momentum / mass + glm::cross(angularMomentum, offset);
					
					/*
					std::cout << velAt.x << " ";
					std::cout << velAt.y << " ";
					std::cout << velAt.z << " ";
					std::cout << "\n";
					*/

					float v = length(velAt);
					if (v > 0.001f)
					{
						glm::vec3 drag = -normalize(velAt) * (0.5f * v * v * (1000.f/float(vr*vr)) );
						torques -= glm::cross(drag, pos - boatPos);
						forces += drag;
					}
				}
			}
		}
	}
	if (Window::keyDown(GLFW_KEY_UP))
	{
		glm::vec3 force = orientation * glm::vec3(700, 0, 0);
		glm::vec3 offset = orientation * glm::vec3(-0.5, -0.1, 0);
		forces += force;
		torques -= glm::cross(force, offset);
	}
	if (Window::keyDown(GLFW_KEY_RIGHT))
	{
		glm::vec3 force = orientation * glm::vec3(0, 0, -100);
		glm::vec3 offset = orientation * glm::vec3(-0.5, -0.1, 0);
		forces += force;
		torques -= glm::cross(force, offset);
	}
	if (Window::keyDown(GLFW_KEY_LEFT))
	{
		glm::vec3 force = orientation * glm::vec3(0, 0, 100);
		glm::vec3 offset = orientation * glm::vec3(-0.5, -0.1, 0);
		forces += force;
		torques -= glm::cross(force, offset);
	}
	
	forces.y -= 9.82f*mass;
	
	momentum += forces * dt;
	angularMomentum += torques * dt;

	boatVel = momentum / mass;
	glm::mat3 rotation = glm::mat3_cast(orientation);
	glm::vec3 angularVelocity = angularMomentum;

	glm::quat angularVelocityQ(
		0,
		angularVelocity
	);

	glm::quat spin = 0.5f*angularVelocityQ * orientation;

	boatPos += boatVel * dt;
	orientation += spin * dt;
	orientation = glm::normalize(orientation);

	/*
	std::cout << boatPos.x << " ";
	std::cout << boatPos.y << " ";
	std::cout << boatPos.z << " ";
	std::cout << "\n";

	std::cout << forces.x << " ";
	std::cout << forces.y << " ";
	std::cout << forces.z << " ";
	std::cout << "\n";
	std::cout << "///////////////////////////////////\n";
	*/

	forces = glm::vec3(0);
	torques = glm::vec3(0);




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
		//m->render(modelShader);
	}
	
	voxelShader.use();
	voxelShader.uniform("viewProj", cameraTransform);
	voxelShader.uniform("fov", camera.fov);
	voxelShader.uniform("position", boatPos);
	voxelShader.uniform("rotation", glm::mat3_cast(orientation));
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
