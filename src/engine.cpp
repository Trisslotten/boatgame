#include "engine.hpp"
#include <iostream>
#include "input.hpp"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "window.hpp"

void PlayingState::init()
{
	renderer.init();

	computeTest.add("test.comp");
	computeTest.compile();


	
}

void PlayingState::update(double dt)
{
	elapsed = globalTimer.elapsed();

	if(Window::inFocus())
		camera.update(dt);
	renderer.setCamera(camera);
	
}

void PlayingState::render()
{
	renderer.setGlobalTime(elapsed);

	//renderer.submit(model);

	renderer.render();
}
