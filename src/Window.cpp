#include "stdafx.hpp"
#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

namespace MEW {


	Window::Window(Window&& other) {
		window_ = other.window_;
		other.window_ = nullptr;
	};
	bool Window::isOpen()
	{
		return true;
	}

	void Window::swapBuffer()
	{
		glfwSwapBuffers(window_);
	}

	bool Window::closedPressed() {
		return glfwWindowShouldClose(window_);
	}

	Window::~Window() {

		if (isDeletable_ && nullptr != window_) {
			glfwDestroyWindow(window_);
		}
		else {
			isDeletable_ = true;
		}

	}

}