#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"

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

	MEW::Shader shader("../data/example.vs","../data/example.fs");
	
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
	MEW::Object obj(pointvertex,&shader);
	MEW::Object obj2(pointvertex2,&shader);
	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj.UseProgram();
		shader.setFloat3("ourColor", color);

		obj.Draw();

		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);

		if (w.isKeyPressed('W')) obj2.TranslateY(deltaTime * 1);
		if (w.isKeyPressed('A')) obj2.TranslateX(deltaTime * -1);
		if (w.isKeyPressed('S')) obj2.TranslateY(deltaTime * -1);
		if (w.isKeyPressed('D')) obj2.TranslateX(deltaTime * 1);
		if (w.isKeyPressed('Q')) obj2.RotateY(90 * deltaTime);
		if (w.isKeyPressed('E')) obj2.RotateY(-90 * deltaTime);
		if (w.isKeyPressed('Z')) obj2.Scale(glm::vec3(1 * deltaTime));
		if (w.isKeyPressed('X')) obj2.Scale(glm::vec3(-1 * deltaTime));

		obj2.Draw();

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}