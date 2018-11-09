#include "engine.hpp"
#include <iostream>
#include "input.hpp"
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "window.hpp"

void TestState::init()
{
	renderer.init();
	
	for (int i = 0; i < 0; i++)
	{
		float x = 2000.f*(rand() / float(RAND_MAX));
		float y = 2000.f*(rand() / float(RAND_MAX));
		ps.emplace_back(x, 0.f, y);
	}

	elapsed = globalTimer.elapsed();
	for (auto& p : ps)
	{
		p.y = water.heightAt(p, elapsed);
	}
	

	//float h = 0;
	//ps.emplace_back(1, h, 1);
	//ps.emplace_back(2, h, 1);
	//ps.emplace_back(1, h, 2);
	//ps.emplace_back(2, h, 2);
	//ps.emplace_back(1, h + 1, 1);
	//ps.emplace_back(2, h + 1, 1);
	//ps.emplace_back(1, h + 1, 2);
	//ps.emplace_back(2, h + 1, 2);

}

void TestState::update(double dt)
{
	if(Window::inFocus())
		camera.update(dt);
	renderer.setCamera(camera);

	elapsed = globalTimer.elapsed();
	
	
	for (auto& p : ps)
	{
		//glm::vec3 v = water.velocityAt(p, elapsed);
		//p += v * float(dt);
	}
	
	
}

void TestState::render()
{
	renderer.setGlobalTime(elapsed);

	
	for (const auto& p : ps)
	{
		renderer.drawPoint(p);
	}
	


	renderer.render();
}
