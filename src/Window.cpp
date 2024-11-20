#include "stdafx.hpp"
#include "mew/Window.hpp"
#include "GLFW/glfw3.h"
#include <cctype>

namespace MEW {


	Window::Window(Window&& other) {
		window_ = other.window_;
		other.window_ = nullptr;
	};
	bool Window::isOpen()
	{
		return true;
	}

	void Window::newframe(const float* color) 
	{
		currentFrame_ = glfwGetTime();
		deltaTime_ = currentFrame_ - lastFrame_;
		lastFrame_ = currentFrame_;
		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	double Window::deltaTime() {
		return deltaTime_;
	}

	void Window::endWindowFrame()
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	bool Window::isKeyPressed(char key) {
		return glfwGetKey(window_, toupper(key)) == GLFW_PRESS;
	}

	bool Window::isKeyPressed(int key) {
		return glfwGetKey(window_, key) == GLFW_PRESS;
	}

	double Window::time() {
		return glfwGetTime();
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