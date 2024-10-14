#ifndef __WINDOW_H__
#define __WINDOW_H__ 1
#include <optional>
#include <string>
#include "GLFW/glfw3.h"


namespace MEW {

	class WindowSystem {
	public:
		bool isDeletable_;
		static std::optional<WindowSystem> make() {

			if (!glfwInit()) {
				return std::nullopt;
			}

			return WindowSystem{ };
		};
		~WindowSystem(){ 
			if (isDeletable_) {
				glfwTerminate();
			}
			else {
				isDeletable_ = true;
			}
		}
		WindowSystem(WindowSystem&) {
			isDeletable_ = true;
		}

		WindowSystem(const WindowSystem&) = delete;
		WindowSystem& operator=(const WindowSystem&) = delete;

		WindowSystem(WindowSystem&& other) 
			: isDeletable_(other.isDeletable_) {
			other.isDeletable_ = false; // Transfer ownership
		}
	private:
		WindowSystem() : isDeletable_(true) {}
	};


	class Window {
	public:
		GLFWwindow* window_;
		bool isDeletable_;
		static std::optional<Window> make(int x, int y, std::string& name, WindowSystem&) {
			auto w = glfwCreateWindow(x, y, name.c_str(), nullptr, nullptr);
			if (nullptr == w) return std::nullopt;
			glfwMakeContextCurrent(w);
			return Window{ w };
		}

		bool isOpen();
		bool isKeyPressed(char key);
		bool isKeyPressed(int key);
		void endWindowFrame();
		void newframe(const float*);
		bool closedPressed();
		double time();
		double deltaTime();
		Window(Window&& other); //CONSTRUCTOR DE MOVIMIENTO
		Window(Window& other) { isDeletable_ = true; this->window_ = other.window_; other.window_ = nullptr; other.isDeletable_ = true; };

		//Window(const Window&) {};
		~Window();
	private:
		double currentFrame_;
		double deltaTime_;
		double lastFrame_;
		Window(GLFWwindow* w) : window_{ w } ,isDeletable_(false) {  }
		Window& operator=(const Window&) = delete;
		Window& operator=( Window&&) = delete; //ASIGNACION DE MOVIMIENTO
	};

}
#endif //__WINDOW__