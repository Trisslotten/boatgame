#include "renderer.hpp"

#include <iostream>
#include "input.hpp"
#include "window.hpp"
#include "lodepng.h"
#include <glm/gtx/rotate_vector.hpp>

namespace
{
	const GLfloat skyboxVerts[] =
	{
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

	const GLfloat boatVerts[] = 
	{
		0.f, 0.5f, 1.f,
		-1.f,0.5f,-1.f,
		0.f,-0.5f,-1.f,

		0.f, 0.5f, 1.f,
		1.f,0.5f,-1.f,
		0.f,-0.5f,-1.f,

		-1.f,0.5f,-1.f,
		0.f,-0.5f,-1.f,
		1.f,0.5f,-1.f,
	};

	const GLfloat quadVerts[] =
	{
		//(x,y,z) (u,v)
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
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

	waterShader.add("water.vert");
	waterShader.add("water.tesc");
	waterShader.add("water.tese");
	waterShader.add("water.frag");
	waterShader.compile();


	glGenVertexArrays(1, &pointVAO);
	glBindVertexArray(pointVAO);
	glGenBuffers(1, &pointVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	pointShader.add("point.vert");
	pointShader.add("point.frag");
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVerts), &skyboxVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	skyboxShader.add("skybox.vert");
	skyboxShader.add("skybox.frag");
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


	glGenVertexArrays(1, &boatVAO);
	glBindVertexArray(boatVAO);
	glGenBuffers(1, &boatVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boatVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boatVerts), &boatVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	boatShader.add("boat.vert");
	boatShader.add("boat.tesc");
	boatShader.add("boat.tese");
	boatShader.add("boat.frag");
	boatShader.compile();


	glGenFramebuffers(1, &waveFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, waveFramebuffer);
	glGenTextures(1, &waveTexture);
	glBindTexture(GL_TEXTURE_2D, waveTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WAVE_RES, WAVE_RES, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waveTexture, 0);
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Could not create waveFramebuffer\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	waveShader.add("wave.vert");
	waveShader.add("wave.frag");
	waveShader.compile();
}


float Renderer::skyGamma(float z, float a)
{
	return acos(sin(zenith)*sin(z)*cos(a - azimuth) + cos(zenith)*cos(z));
}
float perez(float z, float g, float coeffs[5])
{
	return	(1.0 + coeffs[0]*exp(coeffs[1] / cos(z)))*
		(1.0 + coeffs[2]*exp(coeffs[3]*g) + coeffs[4]*pow(cos(g), 2.0));
}
glm::vec3 rgb(float Y, float x, float y)
{
	float X = x / y * Y;
	float Z = (1. - x - y) / y * Y;
	glm::vec3 result;
	result.r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
	result.g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
	result.b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;
	return result;
}

void Renderer::calcSkyValues()
{
	// https://nicoschertler.wordpress.com/2013/04/03/simulating-a-days-sky/
	float pi = glm::pi<float>();
	// 1 => straight up, 0 => on horizon
	float sunpos = 0.65;
	zenith = 0.5 * pi * (1.0-sunpos);
	azimuth = pi/8;

	glm::vec4 sun{ 0, 1, 0, 0 };
	sun = glm::rotateX(sun, zenith);
	sun = glm::rotateY(sun, azimuth);
	sunDir = glm::vec3(sun);

	float Yz = (4.0453f * turbidity - 4.9710f) 
		* glm::tan((4.0f / 9.f - turbidity / 120.0f) * (pi - 2.f * zenith)) 
		- 0.2155f * turbidity + 2.4192f;
	float  Y0 = (4.0453f * turbidity - 4.9710f) 
		* glm::tan((4.0f / 9.f - turbidity / 120.0f) * (pi)) 
		- 0.2155f * turbidity + 2.4192f; ;
	this->Yz = (Yz / Y0);
	float  z3 = glm::pow(zenith, 3.f);
	float  z2 = zenith * zenith;
	float  z = zenith;
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


	float g = skyGamma(zenith, azimuth);
	float Yp = Yz * perez(z, g, skyCoeffsY) / perez(0, zenith, skyCoeffsY);
	float xp = xz * perez(z, g, skyCoeffsx) / perez(0, zenith, skyCoeffsx);
	float yp = yz * perez(z, g, skyCoeffsy) / perez(0, zenith, skyCoeffsy);
	sunColor = rgb(Yp, xp, yp);
	sunColor = glm::clamp(sunColor, 0.0f, 1.0f);
}

void Renderer::render()
{
	if (Input::isKeyDown(GLFW_KEY_F5))
	{
		waterShader.reload();
		skyboxShader.reload();
		pointShader.reload();
	}
	calcSkyValues();


	// render wave displacement to texture
	glBindFramebuffer(GL_FRAMEBUFFER, waveFramebuffer);
	glViewport(0, 0, WAVE_RES, WAVE_RES);
	glClearColor(0.f, 0.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);
	waveShader.use();
	waveShader.uniform("waveTileSize", WAVE_TILE_SIZE);
	waveShader.uniform("time", globalTime);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDepthFunc(GL_LESS);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	auto wSize = Window::size();
	glViewport(0, 0, wSize.x, wSize.y);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 camTransform = camera.getTransform();

	glDisable(GL_CULL_FACE);
	boatShader.use();
	boatShader.uniform("viewProj", camTransform);
	boatShader.uniform("sunDir", sunDir);
	glBindVertexArray(boatVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawArrays(GL_PATCHES, 0, 6);
	glEnable(GL_CULL_FACE);


	// render water
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterNormalTex);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, waveTexture);
	waterShader.use();
	waterShader.uniform("numPatches", float(MAX_PATCHES));
	waterShader.uniform("normalMap", 0);
	waterShader.uniform("waveMap", 1);
	waterShader.uniform("size", waterSize);
	waterShader.uniform("viewProj", camTransform);
	waterShader.uniform("cameraPos", camera.position);
	waterShader.uniform("time", globalTime);
	waterShader.uniform("windowSize", wSize);
	waterShader.uniform("fov", camera.fov);
	for (int i = 0; i < 5; i++)
	{
		char chr = 'A' + i;
		waterShader.uniform(std::string("skyCoeffsY.") + chr, skyCoeffsY[i]);
		waterShader.uniform(std::string("skyCoeffsx.") + chr, skyCoeffsx[i]);
		waterShader.uniform(std::string("skyCoeffsy.") + chr, skyCoeffsy[i]);
	}
	waterShader.uniform("turbidity", turbidity);
	waterShader.uniform("zenith", zenith);
	waterShader.uniform("azimuth", azimuth);
	waterShader.uniform("Yz", Yz);
	waterShader.uniform("xz", xz);
	waterShader.uniform("yz", yz);
	waterShader.uniform("sunDir", sunDir);
	waterShader.uniform("sunColor", sunColor);
	waterShader.uniform("cameraDir", camera.getLookDir());
	waterShader.uniform("waveTileSize", WAVE_TILE_SIZE);
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
	skyboxShader.uniform("turbidity", turbidity);
	skyboxShader.uniform("zenith", zenith);
	skyboxShader.uniform("azimuth", azimuth);
	skyboxShader.uniform("Yz", Yz);
	skyboxShader.uniform("xz", xz);
	skyboxShader.uniform("yz", yz);
	skyboxShader.uniform("time", globalTime);
	skyboxShader.uniform("resolution", wSize);
	skyboxShader.uniform("sunDir", sunDir);
	skyboxShader.uniform("sunColor", sunColor);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);

}


