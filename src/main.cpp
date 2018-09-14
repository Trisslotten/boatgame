#include "window.hpp"
#include <glm/glm.hpp>


int main(void)
{
	Window::setTitle("boatgame");
	Window::open();

	while (!Window::shouldClose())
	{

		
		Window::update();
	}

	return 0;
}