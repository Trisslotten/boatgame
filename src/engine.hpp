#pragma once
#include <iostream>
#include "timer.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "water.hpp"
#include "boat.hpp"

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


class PlayingState : public EngineState
{
	std::vector<Part> parts;
	Boat boat;

	Timer globalTimer;
	float elapsed;

	Camera camera;
	Renderer renderer;

	ShaderProgram computeTest;
public:
	void init() override;
	void update(double dt) override;
	void render() override;
};