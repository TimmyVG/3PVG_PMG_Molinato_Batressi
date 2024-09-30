#include "stdafx.hpp"
#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

namespace MEW {


	Window::Window() {
		
	}

	int Window::WindowInit(int width, int height, const char* title)
	{
		if (!glfwInit()) return -1;

		window_ = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!window_) {
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(window_);

		return 0;
	}

}