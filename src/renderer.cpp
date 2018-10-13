#include "renderer.hpp"

#include <iostream>
#include "input.hpp"
#include "window.hpp"
#include "lodepng.h"

namespace
{
	GLfloat skybox_verts[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
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

	waterShader.add(GL_VERTEX_SHADER,          "water.vert");
	waterShader.add(GL_TESS_CONTROL_SHADER,    "water.tesc");
	waterShader.add(GL_TESS_EVALUATION_SHADER, "water.tese");
	waterShader.add(GL_FRAGMENT_SHADER,        "water.frag");
	waterShader.compile();


	glGenVertexArrays(1, &pointVAO);
	glBindVertexArray(pointVAO);
	glGenBuffers(1, &pointVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	pointShader.add(GL_VERTEX_SHADER, "point.vert");
	pointShader.add(GL_FRAGMENT_SHADER, "point.frag");
	pointShader.compile();


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


	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_verts), &skybox_verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	skyboxShader.add(GL_VERTEX_SHADER, "skybox.vert");
	skyboxShader.add(GL_FRAGMENT_SHADER, "skybox.frag");
	skyboxShader.compile();

	skyCoeffsY[0] = 0.1787f * turbidity - 1.4630f;
	skyCoeffsY[1] = -0.3554f * turbidity + 0.4275f;
	skyCoeffsY[2] = -0.0227f * turbidity + 5.3251f;
	skyCoeffsY[3] = 0.1206f * turbidity - 2.5771f;
	skyCoeffsY[4] = -0.0670f * turbidity + 0.3703f;

	skyCoeffsx[0] = -0.0193f * turbidity - 0.2592f;
	skyCoeffsx[1] = -0.0665f * turbidity + 0.0008f;
	skyCoeffsx[2] = -0.0004f * turbidity + 0.2125f;
	skyCoeffsx[3] = -0.0641f * turbidity - 0.8989f;
	skyCoeffsx[4] = -0.0033f * turbidity + 0.0452f;

	skyCoeffsy[0] = -0.0167f * turbidity - 0.2608f;
	skyCoeffsy[1] = -0.0950f * turbidity + 0.0092f;
	skyCoeffsy[2] = -0.0079f * turbidity + 0.2102f;
	skyCoeffsy[3] = -0.0441f * turbidity - 1.6537f;
	skyCoeffsy[4] = -0.0109f * turbidity + 0.0529f;
}

void Renderer::calcZenitalAbsolutes()
{
	// https://nicoschertler.wordpress.com/2013/04/03/simulating-a-days-sky/
	double pi = glm::pi<double>();
	zenith = pi / 8;
	azimuth = 0;

	double Yz = (4.0453 * turbidity - 4.9710) * glm::tan((4.0 / 9 - turbidity / 120.0) * (pi - 2 * zenith)) - 0.2155 * turbidity + 2.4192;
	double Y0 = (4.0453 * turbidity - 4.9710) * glm::tan((4.0 / 9 - turbidity / 120.0) * (pi)) - 0.2155 * turbidity + 2.4192; ;
	this->Yz = (float)(Yz / Y0);
	double z3 = (float)glm::pow(zenith, 3);
	double z2 = zenith * zenith;
	double z = zenith;
	glm::vec3 T_vec{ turbidity * turbidity, turbidity, 1 };
	glm::vec3 x
	{
		0.00166f * z3 - 0.00375f * z2 + 0.00209f * z,
		-0.02903f * z3 + 0.06377f * z2 - 0.03202f * z + 0.00394f,
		0.11693f * z3 - 0.21196f * z2 + 0.06052f * z + 0.25886f
	};
	this->xz = glm::dot(T_vec, x);
	glm::vec3 y
	{
		0.00275f * z3 - 0.00610f * z2 + 0.00317f * z,
		-0.04214f * z3 + 0.08970f * z2 - 0.04153f * z + 0.00516f,
		0.15346f * z3 - 0.26756f * z2 + 0.06670f * z + 0.26688f
	};
	this->yz = glm::dot(T_vec, y);

}

void Renderer::render()
{
	if (Input::isKeyDown(GLFW_KEY_F5))
		waterShader.reload();

	calcZenitalAbsolutes();

	glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 camTransform = camera.getTransform();


	// render water
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	waterShader.use();
	waterShader.uniform("numPatches", float(MAX_PATCHES));
	waterShader.uniform("normalMap", 0);
	waterShader.uniform("size", waterSize);
	waterShader.uniform("viewProj", camTransform);
	waterShader.uniform("cameraPos", camera.position);
	waterShader.uniform("time", globalTime);
	waterShader.uniform("windowSize", Window::size());
	waterShader.uniform("fov", camera.fov);
	for (int i = 0; i < 5; i++)
	{
		char chr = 'A' + i;
		waterShader.uniform(std::string("skyCoeffsY.") + chr, skyCoeffsY[i]);
		waterShader.uniform(std::string("skyCoeffsx.") + chr, skyCoeffsx[i]);
		waterShader.uniform(std::string("skyCoeffsy.") + chr, skyCoeffsy[i]);
	}
	waterShader.uniform("turbidity", float(turbidity));
	waterShader.uniform("zenith", float(zenith));
	waterShader.uniform("azimuth", float(azimuth));
	waterShader.uniform("Yz", float(Yz));
	waterShader.uniform("xz", float(xz));
	waterShader.uniform("yz", float(yz));

	glBindVertexArray(waterPatchVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glDrawArrays(GL_PATCHES, 0, MAX_PATCHES * MAX_PATCHES);


	// render points
	if (!points.empty())
	{
		glPointSize(5.f);
		glBindVertexArray(pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*points.size(), &points[0], GL_DYNAMIC_DRAW);
		pointShader.use();
		pointShader.uniform("viewProj", camTransform);
		glDrawArrays(GL_POINTS, 0, points.size());
		points.clear();
	}

	
	// render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	skyboxShader.uniform("viewProj", camTransform);
	for (int i = 0; i < 5; i++)
	{
		char chr = 'A' + i;
		skyboxShader.uniform(std::string("skyCoeffsY.") + chr, skyCoeffsY[i]);
		skyboxShader.uniform(std::string("skyCoeffsx.") + chr, skyCoeffsx[i]);
		skyboxShader.uniform(std::string("skyCoeffsy.") + chr, skyCoeffsy[i]);
	}
	skyboxShader.uniform("turbidity", float(turbidity));
	skyboxShader.uniform("zenith", float(zenith));
	skyboxShader.uniform("azimuth", float(azimuth));
	skyboxShader.uniform("Yz", float(Yz));
	skyboxShader.uniform("xz", float(xz));
	skyboxShader.uniform("yz", float(yz));
	skyboxShader.uniform("time", globalTime);
	skyboxShader.uniform("resolution", Window::size());
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}


