#pragma once
#include <iostream>
#include "timer.hpp"
#include "camera.hpp"
#include "renderer.hpp"

class EngineState;

class Engine
{
	EngineState* state = nullptr;
	Timer frame_time;
public:
	Engine();
	void update();
};

class EngineState
{
	friend class Engine;
	EngineState* next_state = nullptr;
public:
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
	Camera camera;
	Renderer renderer;
public:
	void init() override;
	void update(double dt) override;
	void render() override;
};