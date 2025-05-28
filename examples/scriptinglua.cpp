#include "mew/Window.hpp"
#include "mew/Scripting.hpp"
#include "GLFW/glfw3.h"
#include "lua.hpp"
#include "mew/ECSManager.hpp"
#include "mew/World.hpp"
#include "mew/Transform.hpp"
#include "mew/Object.hpp"
#include "mew/Render.hpp"
#include "mew/Input.hpp"


int main() {
	#pragma region Window Creation
	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Ventana";
	auto maybe_w = MEW::Window::make(640, 460, title, ws);
	if (!maybe_w) {
		return -1;
	}

	MEW::Window w = maybe_w.value();

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	#pragma endregion
	int nEntity = 1;
	MEW::ECSManager ecs;
	std::vector<size_t> entities;
	MEW::ScriptingSystem SS;
	std::string script = MEW::file_to_string("../data/scripts/helloworld.lua");
	std::string script2 = MEW::file_to_string("../data/scripts/holamundo.lua");

	MEW::World::GetWorld().setECSManager(&ecs);
	MEW::World::GetWorld().getECSManager()->add_component_type<MEW::ScriptingComponent>();
	MEW::World::GetWorld().getECSManager()->add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();

	MEW::Input input(w.window_);
	MEW::Shader shader("../data/example.vs", "../data/example.fs");

	for (int i = 0; i < nEntity; i++) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::ScriptingComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
		ecs.get_component<MEW::ScriptingComponent>(entity).value().scripts.push_back(script);
	}
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.x = 0;
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.y = 0;
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.z = 0;
  auto& vecS = ecs.get_vectorComponent<MEW::ScriptingComponent>();
	SS.register_functions(vecS);
	SS(vecS);
	input.assign(MEW::Input::Buttons::KEY_A, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_LEFT, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_D, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_RIGHT, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_W, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_UP, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_S, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::KEY_DOWN, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::MOUSE_2, MEW::CAMERA_ROTATE);
	MEW::Camera cameraTest(ecs, 640 / 460);
	while (!done) {
		input.newframe();
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);

		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}

	return 0;
}