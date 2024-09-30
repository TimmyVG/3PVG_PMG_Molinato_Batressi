#ifndef __WINDOW_H__
#define __WINDOW_H__ 1

#include "GLFW/glfw3.h"


namespace MEW {


	class Window {
	public:
		Window();
		int WindowInit(int width, int height, const char* title);
		GLFWwindow* window_;
	};

}
#endif //__WINDOW__