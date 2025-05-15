#include "mew/Window.hpp"
#include "GLFW/glfw3.h"
#include <mew/Inspector.hpp>

int main() {
	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Ventana";
	auto maybe_w = MEW::Window::make(640, 460, title, ws);
	if (!maybe_w) { 
		return -1; 
	}
	MEW::Window w = maybe_w.value();

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };

	MEW::Inspector inspector(w);
	while (!done) {
		w.newframe(backgroundcolor);
		inspector.NewFrame();
		bool show_demo_window = true;
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		inspector.Render();
		



		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
		
		//if (/*algo*/) done = true;
	}

	return 0;
}