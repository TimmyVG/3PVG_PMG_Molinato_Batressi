#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/geometry.hpp"

int WinMain() {
	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Window Example Triangle";
	auto maybe_w = MEW::Window::make(640, 460, title, ws);
	if (!maybe_w) {
		return -1;
	}
	MEW::Window w = maybe_w.value();

	MEW::Shader shader("../data/triangle.vs", "../data/triangle.fs");

	std::vector<float> pointvertex = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f,  0.5f, 0.0f,  // top left 
	};
	std::vector<float> pointvertex2 = {
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left
	};
	MEW::Geometry obj(pointvertex,&shader);
	MEW::Geometry obj2(pointvertex2,&shader);
	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	while (!done) {
		w.newframe(backgroundcolor);

		obj.UseProgram();
		shader.setFloat3("ourColor", color);
		obj.Draw();

		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);

		obj2.Draw();



		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}

	return 0;
}