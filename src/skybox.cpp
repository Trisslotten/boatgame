#include "skybox.hpp"

#include "renderer.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace
{
	GLfloat skyboxVerts[] =
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

	float perez(float z, float g, float coeffs[5])
	{
		return	(1.0f + coeffs[0] * glm::exp(coeffs[1] / glm::cos(z)))*
			(1.0f + coeffs[2] * glm::exp(coeffs[3] * g) + coeffs[4] * pow(glm::cos(g), 2.0f));
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

}

float Skybox::skyGamma(float z, float a)
{
	return acos(sin(zenith)*sin(z)*cos(a - azimuth) + cos(zenith)*cos(z));
}

void Skybox::init()
{
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
}

void Skybox::update()
{
	// https://nicoschertler.wordpress.com/2013/04/03/simulating-a-days-sky/
	float pi = glm::pi<float>();
	// 1: straight up, 0: on horizon
	float sunpos = 0.2;
	zenith = 0.5 * pi * (1.0 - sunpos);
	azimuth = pi / 8;

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

void Skybox::setUniforms(ShaderProgram & shader)
{
	for (int i = 0; i < 5; i++)
	{
		char chr = 'A' + i;
		shader.uniform(std::string("skyCoeffsY.") + chr, skyCoeffsY[i]);
		shader.uniform(std::string("skyCoeffsx.") + chr, skyCoeffsx[i]);
		shader.uniform(std::string("skyCoeffsy.") + chr, skyCoeffsy[i]);
	}
	shader.uniform("zenith", zenith);
	shader.uniform("azimuth", azimuth);
	shader.uniform("Yz", Yz);
	shader.uniform("xz", xz);
	shader.uniform("yz", yz);
	shader.uniform("sunDir", sunDir);
	shader.uniform("sunColor", sunColor);
}

void Skybox::draw(Renderer * renderer)
{
	// render skybox
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	skyboxShader.uniform("viewProj", renderer->getCameraTransform());
	skyboxShader.uniform("time", renderer->getGlobalTime());
	skyboxShader.uniform("resolution", Window::size());
	setUniforms(skyboxShader);
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}
