#include "stdafx.hpp"
#include "mew/Window.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <cctype>

namespace MEW {

	WindowSystem::~WindowSystem() {
		if (isDeletable_) {
			glfwTerminate();
		}
		else {
			isDeletable_ = true;
		}
	}

	std::optional<WindowSystem> WindowSystem::make() {

		if (!glfwInit()) {
			return std::nullopt;
		}

		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100 (WebGL 1.0)
		const char* glsl_version = "#version 100";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
		// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
		const char* glsl_version = "#version 300 es";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

		return WindowSystem{ };
	}


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
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
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

	std::optional<Window> Window::make(int x, int y, std::string& name, WindowSystem&) {
		GLFWwindow* w = glfwCreateWindow(x, y, name.c_str(), nullptr, nullptr);
		if (nullptr == w) return std::nullopt;
		glfwMakeContextCurrent(w);




		return Window{ w };
	}

}