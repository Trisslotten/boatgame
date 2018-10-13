#include "window.hpp"
#include <glm/glm.hpp>
#include "input.hpp"
#include "engine.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"


int main(void)
{
	Window::setTitle("boatgame");
	Window::open();
	Input::initialize();

	EngineState* state = new TestState();
	state->init();;

	Timer frame_time;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplOpenGL3_Init();

	while (!Window::shouldClose())
	{
		Input::reset();

		double dt = frame_time.restart();

		io.DeltaTime = dt;
		io.DisplaySize.x = Window::size().x;
		io.DisplaySize.y = Window::size().y;
		io.MousePos.x = Input::mousePosition().x;
		io.MousePos.y = Input::mousePosition().y;
		io.MouseDown[0] = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_1);
		io.MouseDown[1] = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_2);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Text("Hello, world!");

		state->update(dt);
		state->render();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		

		if (state->next_state)
		{
			auto temp = state->next_state;
			delete state;
			state = temp;
			state->init();
			state->next_state = nullptr;
		}

		Window::update();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
