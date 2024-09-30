#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

int WinMain() {
	MEW::Window window;

	window.WindowInit(640, 400, "Hola Mundo");

	



	

	while (!glfwWindowShouldClose(window.window_)) {
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window.window_);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}