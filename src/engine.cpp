#include "engine.hpp"
#include <iostream>
#include "input.hpp"
#include <GLFW/glfw3.h>

Engine::Engine()
{
	state = new TestState();
	state->init();
}

void Engine::update()
{
	//std::cout << "DEBUG: Engine update\n";
	
	double dt = frame_time.restart();

	state->update(dt);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	state->render();
	

	if (state->next_state)
	{
		auto temp = state->next_state;
		delete state;
		state = temp;
		state->init();
		state->next_state = nullptr;
	}
}

void TestState::init()
{
	std::cout << "DEBUG: TestState init\n";
	float r = rand() / float(RAND_MAX);
	float g = rand() / float(RAND_MAX);
	float b = rand() / float(RAND_MAX);
	glClearColor(r, g, b, 1.f);
}

void TestState::update(double dt)
{

}

void TestState::render()
{

}
