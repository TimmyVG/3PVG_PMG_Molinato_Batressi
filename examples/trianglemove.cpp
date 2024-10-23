#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/Input.hpp"
enum Actions
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ROTATELEFT,
	ROTATERIGHT,
	SIZEUP,
	SIZEDOWN,
};

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
	MEW::Input input(w.window_);
	input.assign(MEW::Input::Keys::KEY_A, LEFT);
	input.assign(MEW::Input::Keys::KEY_D, RIGHT);
	input.assign(MEW::Input::Keys::KEY_W, UP);
	input.assign(MEW::Input::Keys::KEY_S, DOWN);
	input.assign(MEW::Input::Keys::KEY_Q, ROTATELEFT);
	input.assign(MEW::Input::Keys::KEY_E, ROTATERIGHT);
	input.assign(MEW::Input::Keys::KEY_Z, SIZEUP);
	input.assign(MEW::Input::Keys::KEY_X, SIZEDOWN);
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
		input.newframe();
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj.UseProgram();
		shader.setFloat3("ourColor", color);

		obj.Draw();

		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);

		if (input.isKeyPressed(UP)) obj2.TranslateY(deltaTime * 1);
		if (input.isKeyPressed(LEFT)) obj2.TranslateX(deltaTime * -1);
		if (input.isKeyPressed(DOWN)) obj2.TranslateY(deltaTime * -1);
		if (input.isKeyPressed(RIGHT)) obj2.TranslateX(deltaTime * 1);
		if (input.isKeyPressed(ROTATERIGHT)) obj2.RotateZ(90 * deltaTime);
		if (input.isKeyPressed(ROTATELEFT)) obj2.RotateZ(-90 * deltaTime);
		if (input.isKeyPressed(SIZEUP)) obj2.Scale(glm::vec3(1 * deltaTime));
		if (input.isKeyPressed(SIZEDOWN)) obj2.Scale(glm::vec3(-1 * deltaTime));
		/*
		std::vector<double> tmp = input.getMousePos();
		glm::vec3 mousepos(tmp[0], tmp[1], 0.0f);
		obj.SetTranslation(mousepos);
		*/
		obj2.Draw();

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}