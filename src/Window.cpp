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

	void Window::clear(const float* color) 
	{
		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Window::endWindowFrame()
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();
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