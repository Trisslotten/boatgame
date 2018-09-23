#include "renderer.hpp"

#include "window.hpp"

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

}

void Renderer::render()
{
	glClearColor(47.f / 255, 141.f / 255, 255.f / 255, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	waterShader.use();
	waterShader.uniform("numPatches", float(MAX_PATCHES));
	waterShader.uniform("size", waterSize);
	waterShader.uniform("viewProj", camera.getTransform());
	waterShader.uniform("cameraPos", camera.position);
	waterShader.uniform("time", float(timer.elapsed()));
	waterShader.uniform("windowSize", Window::size());
	glBindVertexArray(waterPatchVAO);
	glPatchParameteri(GL_PATCH_VERTICES, 1);

	glDrawArrays(GL_PATCHES, 0, MAX_PATCHES * MAX_PATCHES);

}
