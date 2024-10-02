#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

int WinMain() {
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
	auto w = maybe_w.value();

	bool done = false;
	while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);

		w.swapBuffer();

		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;

		glfwPollEvents();
		//if (/*algo*/) done = true;
	}

	return 0;
}