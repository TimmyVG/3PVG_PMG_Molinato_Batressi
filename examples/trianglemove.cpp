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

	MEW::Shader shader("../data/example.vs", "../data/example.fs");

	MEW::Object obj(std::string("../data/miku/source/Miku.fbx"), &shader);

	MEW::Object obj2(std::string("../data/Silla.fbx"), &shader);

	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };

	obj2.TranslateZ(-10);
	obj2.TranslateX(-3);
	obj2.TranslateY(-3);

	obj2.RotateX(-45.0f);

	obj2.SetScale(glm::vec3(1.0f));
	obj.TranslateZ(-10);
	obj.RotateX(-45.0f);

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	while (!done) {
		input.newframe();
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj.UseProgram();
		shader.setFloat3("ourColor", color);


		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);

		if (input.isKeyPressed(UP)) obj2.TranslateY(deltaTime * 1);
		if (input.isKeyPressed(LEFT)) obj2.TranslateX(deltaTime * -1);
		if (input.isKeyPressed(DOWN)) obj2.TranslateY(deltaTime * -1);
		if (input.isKeyPressed(RIGHT)) obj2.TranslateX(deltaTime * 1);
		if (input.isKeyPressed(ROTATERIGHT)) obj2.RotateZ(1 * deltaTime);
		if (input.isKeyPressed(ROTATELEFT)) obj2.RotateZ(-1 * deltaTime);
		if (input.isKeyDown(ATTACK)) obj2.Scale(glm::vec3(1 * deltaTime));
		if (input.isKeyPressed(ATTACK2)) obj2.Scale(glm::vec3(-1 * deltaTime));
		
		float normalizedX = (input.getMousePos().x / w.getWindowWidth()) * 2.0f - 1.0f;
		float normalizedY = (input.getMousePos().y / w.getWindowHeight()) * 2.0f - 1.0f;
		normalizedY = -normalizedY; 

		glm::vec3 objectPosition = obj.GetTranslation();

		obj.SetTranslation(glm::vec3(normalizedX , normalizedY, objectPosition.z));

		// Draw the object
		obj.Draw();

		obj2.Draw();

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}