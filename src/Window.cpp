#include "stdafx.hpp"
#include "mew/Window.hpp"
#include "GLFW/glfw3.h"

namespace MEW {


	Window::Window(int width, int height, const char* title) {
		width_ = width;
		height_ = height;
		title_ = title;


		window_ = glfwCreateWindow(width_, height_, title_, NULL, NULL);

		if (nullptr == window_) {
			glfwTerminate();
			//algo
		}

		//glfwMakeContextCurrent(window_);

		//algo
	}




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
		glfwDestroyWindow(window_);

	}

}