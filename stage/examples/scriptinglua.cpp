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
	int nEntity = 5;
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
	MEW::RenderSystem RS;

	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/miku/source/Miku.fbx");
	objmiku.model->loadMeshes();

	size_t miku = ecs.create_entity();
	entities.push_back(miku);
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);
	*ecs.get_component<MEW::RenderComponent>(miku).value().object = objmiku;

	for (int i = 0; i < nEntity; i++) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		//MEW::ScriptingComponent tempComponent;
		//tempComponent.scripts.push_back(script2);
		ecs.add_component<MEW::ScriptingComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
		//ecs.get_component<MEW::ScriptingComponent>(entity).value() = std::move(tempComponent);
		ecs.get_component<MEW::ScriptingComponent>(entity).value().scripts.push_back(script);
	}
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.x = 0;
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.y = 0;
	ecs.get_component<MEW::TransformComponent>(entities.at(0)).value().translation_.z = 0;
  auto& vecS = ecs.get_vectorComponent<MEW::ScriptingComponent>();
	SS.add_global(vecS, "multiplication", MEW::multiplication);
	SS.add_global(vecS, "GetPosition", MEW::lua_get_position);
	SS.add_global(vecS, "SetPosition", MEW::lua_set_position);
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
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		MEW::RenderSystemUnlit()(vecT, vecR, RS, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());

		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}

	return 0;
}