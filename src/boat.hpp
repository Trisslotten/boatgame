#pragma once

#include "model.hpp"


struct PartConfig
{
	std::string model;
};

struct Part
{
	Model* model;

	//glm::vec3 bounds;

	GLuint voxelTex;

	Part(Model* model): model(model)
	{
		//bounds = model->getBounds();
	}

};


class Boat
{



};