#ifndef __WINDOW_H__
#define __WINDOW_H__ 1
#include <optional>
#include <string>
#include "gl/glew.h"
#include "GLFW/glfw3.h"


namespace MEW {

	class WindowSystem {
	public:
		bool isDeletable_;
		static std::optional<WindowSystem> make();
		~WindowSystem();
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
		static std::optional<Window> make(int x, int y, std::string& name, WindowSystem&);

		bool isOpen();
		bool isKeyPressed(char key);
		bool isKeyPressed(int key);
		void endWindowFrame();
		void newframe(const float*);
		bool closedPressed();
		double time();
		double deltaTime();
		int getWindowHeight();
		int getWindowWidth();
		Window(Window&& other) noexcept; //CONSTRUCTOR DE MOVIMIENTO
		Window(Window& other);


		//Window(const Window&) {};
		~Window();
	private:
		double currentFrame_;
		double deltaTime_;
		double lastFrame_;
		int window_width;
		int window_height;
		Window(GLFWwindow* w) : window_{ w } ,isDeletable_(false), window_width(0),window_height(0) {  }
		Window& operator=(const Window&) = delete;
		Window& operator=( Window&&) = delete; //ASIGNACION DE MOVIMIENTO
	};

}
#endif //__WINDOW__