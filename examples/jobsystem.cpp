#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/Input.hpp"
#include "mew/JobSystem.hpp"



int WinMain() {
	JobSystem js;
	std::mutex output_mutex;

	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Window Example Job";
	auto maybe_w = MEW::Window::make(640, 460, title, ws);
	if (!maybe_w) {
		return -1;
	}
	MEW::Window w = maybe_w.value();

	MEW::Shader shader("../data/example.vs", "../data/example.fs");

	MEW::Object obj2(std::string("../data/Silla.fbx"), &shader);

	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };

	obj2.TranslateZ(-10);
	obj2.TranslateY(-3);

	obj2.RotateX(-45.0f);

	obj2.SetScale(glm::vec3(1.0f));

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;

	auto prueba = js.add([]() {
		return changeSize();  // Modify obj2's size
		});

	bool chair_loaded = false;

	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj2.UseProgram();
		shader.setFloat3("ourColor", color2);
		if (prueba.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			chair_loaded = true;
		}
		
		if (chair_loaded)
		{
			obj2.Draw();
		}


		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}