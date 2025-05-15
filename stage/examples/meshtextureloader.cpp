#include "GLFW/glfw3.h"
#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 


int global = 0;


int WinMain() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();


	MEW::TransformSystem TS;
	MEW::RenderSystem RS;



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
	MEW::Shader shader("../data/example.vs", "../data/example.fs");

	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/miku/source/Miku.fbx");
	objmiku.model->loadMeshes();

	MEW::Object objsilla(&shader);
	objsilla.model->loadModel("../data/silla/WoodenChair_low.fbx");
	objsilla.model->loadMeshes();

	std::vector<size_t> entities;

		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::RenderComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
		MEW::RenderComponent* rc = &ecs.get_component<MEW::RenderComponent>(entity).value();
		*rc->object = objsilla;
		TS.Translate(glm::vec3((rand() % 50) - 25.0f, (rand() % 30) - 15.0f, -50.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());
	



	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();


		if (w.isKeyPressed('W')) TS.TranslateY(static_cast<float>(deltaTime) * 1, &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('A')) TS.TranslateX(static_cast<float>(deltaTime) * -1, &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('S')) TS.TranslateY(static_cast<float>(deltaTime) * -1, &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('D')) TS.TranslateX(static_cast<float>(deltaTime) * 1, &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('Q')) TS.RotateX(1 * static_cast<float>(deltaTime), &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('E')) TS.RotateX(-1 * static_cast<float>(deltaTime), &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('Z')) TS.Scale(glm::vec3(1 * static_cast<float>(deltaTime)), &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		if (w.isKeyPressed('X')) TS.Scale(glm::vec3(-1 * static_cast<float>(deltaTime)), &ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());


			RS.Draw(&ecs.get_component<MEW::RenderComponent>(entities.at(0)).value(),
				&ecs.get_component<MEW::TransformComponent>(entities.at(0)).value());
		
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}