#pragma once
#include <iostream>
#include "timer.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "water.hpp"

class EngineState
{
public:
	EngineState* next_state = nullptr;
	virtual void init() = 0;
	virtual void update(double dt) = 0;
	virtual void render() = 0;

	void changeState(EngineState* new_state)
	{
		if (next_state)
		{
			std::cout << "ERROR: calling EngineState::changeState() more than once on frame\n";
			delete new_state;
		}
		else
		{
			next_state = new_state;
		}
	}
};


class TestState : public EngineState
{
	std::vector<glm::vec3> ps;

	Timer globalTimer;
	float elapsed;

	Water water;
	Camera camera;
	Renderer renderer;
public:
	void init() override;
	void update(double dt) override;
	void render() override;
};