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
			isDeletable_ = false;
		}
	private:
		//WindowSystem() { isDeletable_ = true; }
		//WindowSystem(bool* isD) : isDeletable_{ isD } {};
	};


	class Window {
	public:
		GLFWwindow* window_;
		static std::optional<Window> make(int x, int y, std::string& name, WindowSystem&) {
			auto w = glfwCreateWindow(x, y, name.c_str(), nullptr, nullptr);
			if (nullptr == w) return std::nullopt;
			return Window{ w };
		}

		bool isOpen();
		void swapBuffer();
		bool closedPressed();
		Window(Window&& other); //CONSTRUCTOR DE MOVIMIENTO
		Window(const Window&) {};
		~Window();
	private:

		Window(GLFWwindow* w) : window_{ w } {}
		Window& operator=(const Window&) = delete;
		Window& operator=( Window&&) = delete; //ASIGNACION DE MOVIMIENTO
	};

}
#endif //__WINDOW__