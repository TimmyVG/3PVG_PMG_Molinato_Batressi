#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Input.hpp"
#include "mew/geometry.hpp"
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
	ATTACK,
	ATTACK2,
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

	MEW::Input input(w.window_);
	input.assign(MEW::Input::Buttons::KEY_A, LEFT);
	input.assign(MEW::Input::Buttons::KEY_LEFT, LEFT);
	input.assign(MEW::Input::Buttons::KEY_D, RIGHT);
	input.assign(MEW::Input::Buttons::KEY_RIGHT, RIGHT);
	input.assign(MEW::Input::Buttons::KEY_W, UP);
	input.assign(MEW::Input::Buttons::KEY_UP, UP);
	input.assign(MEW::Input::Buttons::KEY_S, DOWN);
	input.assign(MEW::Input::Buttons::KEY_DOWN, DOWN);
	input.assign(MEW::Input::Buttons::KEY_Q, ROTATELEFT);
	input.assign(MEW::Input::Buttons::KEY_E, ROTATERIGHT);
	input.assign(MEW::Input::Buttons::KEY_Z, SIZEUP);
	input.assign(MEW::Input::Buttons::KEY_X, SIZEDOWN);
	input.assign(MEW::Input::Buttons::MOUSE_1, ATTACK);
	input.assign(MEW::Input::Buttons::MOUSE_2, ATTACK2);

	MEW::Shader shader("../data/triangle.vs", "../data/triangle.fs");
	
	std::string mikudirectory = "../data/miku/source/Miku.fbx";

	std::vector<float> pointvertex = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f,  0.5f, 0.0f,  // top left 
	};

	MEW::Geometry obj2(pointvertex,&shader);

	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	obj2.SetScale(glm::vec3(1.0f));

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	while (!done) {
		input.newframe();
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);

		if (input.isKeyPressed(UP)) obj2.TranslateY(static_cast<float>(deltaTime) * 1.0f);
		if (input.isKeyPressed(LEFT)) obj2.TranslateX(static_cast<float>(deltaTime) * -1.0f);
		if (input.isKeyPressed(DOWN)) obj2.TranslateY(static_cast<float>(deltaTime) * -1.0f);
		if (input.isKeyPressed(RIGHT)) obj2.TranslateX(static_cast<float>(deltaTime) * 1.0f);
		if (input.isKeyPressed(ROTATERIGHT)) obj2.RotateZ(1.0f * static_cast<float>(deltaTime));
		if (input.isKeyPressed(ROTATELEFT)) obj2.RotateZ(-1.0f * static_cast<float>(deltaTime));
		if (input.isKeyDown(ATTACK)) obj2.Scale(glm::vec3(1.0f * static_cast<float>(deltaTime)));
		if (input.isKeyPressed(ATTACK2)) obj2.Scale(glm::vec3(-1.0f * static_cast<float>(deltaTime)));
		
		float normalizedX = (input.getMousePos().x / w.getWindowWidth()) * 2.0f - 1.0f;
		float normalizedY = (input.getMousePos().y / w.getWindowHeight()) * 2.0f - 1.0f;
		normalizedY = -normalizedY; 

		// Draw the object
		obj2.Draw();

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}