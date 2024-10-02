#ifndef __WINDOW_H__
#define __WINDOW_H__ 1

#include "GLFW/glfw3.h"


namespace MEW {

	class WindowSystem {
	public:
		WindowSystem() {
			if (!glfwInit()) {
				//ALGO
			}
		};
		~WindowSystem(){ glfwTerminate(); }
	};


	class Window {
	public:
		Window(int width, int height, const char* title);
		bool isOpen();
		void swapBuffer();
		bool closedPressed();
		~Window();
	private:
		GLFWwindow* window_;
		int width_;
		int height_;
		const char* title_;
		Window(const Window&);
		Window& operator=(const Window&);
		Window(const Window&&); //CONSTRUCTOR DE MOVIMIENTO
		Window& operator=(const Window&&); //ASIGNACION DE MOVIMIENTO
	};

}
#endif //__WINDOW__