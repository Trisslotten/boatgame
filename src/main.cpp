#include "window.hpp"
#include <glm/glm.hpp>
#include "input.hpp"
#include "engine.hpp"


int main(void)
{
	Window::setTitle("boatgame");
	Window::open();
	Input::initialize();

	Engine engine;

	while (!Window::shouldClose())
	{
		Input::reset();
		engine.update();
		Window::update();
	}

	return 0;
}
