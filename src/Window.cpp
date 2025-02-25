#include "stdafx.hpp"
#include "mew/Window.hpp"

#include "GLFW/glfw3.h"
#include <cctype>

namespace MEW {


	Window::Window(Window&& other) noexcept {
		window_ = other.window_;
		other.window_ = nullptr;
		lastFrame_ = other.lastFrame_;
		window_height = other.window_height;
		window_width = other.window_width;
		currentFrame_ = other.currentFrame_;
		deltaTime_ = other.deltaTime_;

	}
	Window::Window(Window& other)
	{
		isDeletable_ = true;
		this->window_ = other.window_; 
		other.window_ = nullptr; 
		other.isDeletable_ = true; 
		currentFrame_ = other.currentFrame_; 
		deltaTime_ = other.deltaTime_;
	}
	;
	bool Window::isOpen()
	{
		return true;
	}

	void Window::newframe(const float* color) 
	{
		currentFrame_ = glfwGetTime();
		//ImGui_ImplOpenGL3_NewFrame();
		deltaTime_ = currentFrame_ - lastFrame_;
		lastFrame_ = currentFrame_;
		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	double Window::deltaTime() {
		return deltaTime_;
	}

	int Window::getWindowHeight()
	{
		if (window_ != nullptr)glfwGetWindowSize(window_,&window_width,&window_height);
		return window_height;
	}

	int Window::getWindowWidth()
	{
		if (window_ != nullptr)glfwGetWindowSize(window_, &window_width, &window_height);
		return window_width;
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