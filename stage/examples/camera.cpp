#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 
#include <mew/Camera.hpp>
#include "mew/Input.hpp"

int global = 0;

int main() {
	srand(static_cast<unsigned>(time(0)));
	
	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();

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

	MEW::Input input(w.window_);
	MEW::Shader shader("../data/example.vs","../data/example.fs");
	
	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/miku/source/Miku.fbx");
	objmiku.model->loadMeshes();

	MEW::Object objsilla(&shader);
	objsilla.model->loadModel("../data/miku/source/Miku.fbx");
	objsilla.model->loadMeshes();

	std::vector<size_t> entities;
	
	for (int i = 0; i < 100; ++i) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::RenderComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
	
		*ecs.get_component<MEW::RenderComponent>(entity).value().object = objsilla;
		ecs.get_component<MEW::TransformComponent>(entity).value().scale_ = glm::vec3(1);
		ecs.get_component<MEW::TransformComponent>(entity).value().translation_ = glm::vec3((rand() % 50) - 25.0f, (rand() % 30) - 15.0f, -40.0f);
	}
	size_t miku = ecs.create_entity();
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);
	*ecs.get_component<MEW::RenderComponent>(miku).value().object = objmiku;



	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;

	input.assign(MEW::Input::Buttons::KEY_A, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_LEFT, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_D, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_RIGHT, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_W, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_UP, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_S, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::KEY_DOWN, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::MOUSE_2, MEW::CAMERA_ROTATE);
	MEW::Camera cameraTest(ecs,640/460);
	while (!done) {
		input.newframe();
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);
		
		
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		MEW::RenderSystemUnlit()(vecT, vecR, RS, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}
