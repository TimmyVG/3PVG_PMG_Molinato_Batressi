#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

int WinMain() {
	MEW::WindowSystem ws;
	MEW::Window window{ 640, 400, "Hola Mundo"};
	bool done = false;

	//!glfwWindowShouldClose(window.window_)
	while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);

		window.swapBuffer();

		bool closePressed = window.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;

		glfwPollEvents();
		//if (/*algo*/) done = true;
	}

	return 0;
}