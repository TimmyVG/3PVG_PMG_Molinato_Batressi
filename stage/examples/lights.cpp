#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 
#include "mew/Camera.hpp"
#include <stb_image.h>
#include "mew/Inspector.hpp"
#include <mew/Identity.hpp>


int global = 0;






int main() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::LightComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
	ecs.add_component_type<MEW::IdentityComponent>();

	MEW::RenderSystem RS;



	auto maybe_ws = MEW::WindowSystem::make();
	if (!maybe_ws)
	{
		return -1;
	}
	auto ws = maybe_ws.value();
	std::string title = "Window Example Triangle";
	auto maybe_w = MEW::Window::make(1280, 720, title, ws);
	if (!maybe_w) {
		return -1;
	}
	MEW::Window w = maybe_w.value();
	MEW::Shader shader("../data/exampleLight.vs", "../data/exampleLight.fs");
	MEW::Shader shaderDepth("../data/exampleDepth.vs", "../data/exampleDepth.fs");



	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/sponza/sponza.obj");
	objmiku.model->loadMeshes();

	MEW::Object objsilla(&shader);
	objsilla.model->loadModel("../data/silla/WoodenChair_low.fbx");
	objsilla.model->loadMeshes();

	std::vector<size_t> entities;
	size_t miku = ecs.create_entity("miku");
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);
	ecs.get_component<MEW::TransformComponent>(miku).value().scale_ = glm::vec3(0.1f);
	*ecs.get_component<MEW::RenderComponent>(miku).value().object = objmiku;
	for (int i = 0; i < 4; ++i) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::RenderComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);

		*ecs.get_component<MEW::RenderComponent>(entity).value().object = objsilla;

		//TS.Translate(glm::vec3((rand() % 50) - 25.0f, (rand() % 30) - 15.0f, -50.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());
	}




	//Add lights
	MEW::Light directional(ecs, MEW::KTypeLight::Directional);

	//size_t light2 = ecs.create_entity();
	//ecs.add_component<MEW::LightComponent>(light2);
	//ecs.add_component<MEW::TransformComponent>(light2);

	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };


	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;


	MEW::Input input(w.window_);
	input.assign(MEW::Input::Buttons::KEY_A, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_LEFT, MEW::CAMERA_LEFT);
	input.assign(MEW::Input::Buttons::KEY_D, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_RIGHT, MEW::CAMERA_RIGHT);
	input.assign(MEW::Input::Buttons::KEY_W, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_UP, MEW::CAMERA_FORWARD);
	input.assign(MEW::Input::Buttons::KEY_S, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::KEY_DOWN, MEW::CAMERA_BACK);
	input.assign(MEW::Input::Buttons::MOUSE_2, MEW::CAMERA_ROTATE);
	input.assign(MEW::Input::Buttons::MOUSE_2, MEW::ActionsInspector::CLICK_OUT);
	MEW::Camera cameraTest(ecs, 1280 / 720,MEW::CameraType::CAMERA_PERSPECTIVE,
												50.0f,0.1f,5000.0f,10.0f);

	auto getTransform = [cameraTest, &ecs]() {return &ecs.get_component<MEW::TransformComponent>(cameraTest.entity_).value(); };
	auto getComponent = [&ecs]<typename T>(size_t entity) -> std::optional<T> {
		return ecs.get_component<T>(entity).value();
	};

	MEW::Inspector inspector(w);
	inspector.LinkECS(ecs);
	while (!done) {
		input.newframe();
		w.newframe(backgroundcolor);
		inspector.NewFrame();

		deltaTime = w.deltaTime();

		cameraTest.update(deltaTime, input);
		inspector.update(deltaTime, input);

		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& constVecTransform = ecs.get_vectorComponent<MEW::TransformComponent>();
	  auto& VecTransform = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& constVecRender = ecs.get_vectorComponent<MEW::RenderComponent>();
		auto vecC = &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_);
		auto vecCT = &ecs.get_component<MEW::TransformComponent>(cameraTest.entity_);
		auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();

		inspector.WindowEntities();
		//MEW::UpdateLights()(VecTransform, vecL,vecCT);
		MEW::LightSystem()(constVecTransform, constVecRender,vecL, shaderDepth, vecC);
		MEW::RenderSystemLit()(constVecTransform, constVecRender, vecL, shader, vecC, vecCT);
		//MEW::RenderSystemUnlit()(vecT, vecR, RS, shader);

		printf(" pos camera : %f  / %f   / %f\n", vecCT->value().translation_.x,
					vecCT->value().translation_.y,
					vecCT->value().translation_.z );

		



		inspector.Render();

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}
