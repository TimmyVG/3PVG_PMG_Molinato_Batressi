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
#include <mew/Camera.hpp>

int global = 0;






int main() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();

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
	MEW::Shader shader("../data/example.vs","../data/example.fs");
	
	MEW::Object objmiku(&shader);
	objmiku.model->loadModel("../data/miku/source/Miku.fbx");
	objmiku.model->loadMeshes();

	MEW::Object objsilla(&shader);
	objsilla.model->loadModel("../data/silla/WoodenChair_low.fbx");
	objsilla.model->loadMeshes();

	std::vector<size_t> entities;
	
	for (int i = 0; i < 100; ++i) {
		size_t entity = ecs.create_entity();
		entities.push_back(entity);
		ecs.add_component<MEW::RenderComponent>(entity);
		ecs.add_component<MEW::TransformComponent>(entity);
	
		*ecs.get_component<MEW::RenderComponent>(entity).value().object = objsilla;
		TS.Scale(glm::vec3(1), &ecs.get_component<MEW::TransformComponent>(entity).value());
		TS.Translate(glm::vec3((rand() % 50) - 25.0f, (rand() % 30) - 15.0f, -40.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());
	}
	size_t miku = ecs.create_entity();
	ecs.add_component<MEW::RenderComponent>(miku);
	ecs.add_component<MEW::TransformComponent>(miku);
	*ecs.get_component<MEW::RenderComponent>(miku).value().object = objmiku;

	MEW::CameraSystemProjection csp;
	MEW::CameraSystemView csv;

	size_t camera = ecs.create_entity();
	ecs.add_component<MEW::CameraComponent>(camera);
	ecs.add_component<MEW::TransformComponent>(camera);
	ecs.get_component<MEW::CameraComponent>(camera).value().aspectRatio = 640/460;
	ecs.get_component<MEW::CameraComponent>(camera).value().fov = 60.0f;
	ecs.get_component<MEW::CameraComponent>(camera).value().nearPlane = 1.0f;
	ecs.get_component<MEW::CameraComponent>(camera).value().farPlane = 100.0f;
	ecs.get_component<MEW::CameraComponent>(camera).value().type = MEW::CameraType::CAMERA_PERSPECTIVE;
	ecs.get_component<MEW::TransformComponent>(camera)->translation_ = glm::vec3(0, 0, 5);
	csp(ecs.get_vectorComponent<MEW::TransformComponent>(),ecs.get_vectorComponent<MEW::CameraComponent>());
	csv(ecs.get_vectorComponent<MEW::TransformComponent>(), ecs.get_vectorComponent<MEW::CameraComponent>());
	const float color[3] = { 0.25f,0.3f,0.4f };
	const float color2[3] = { 0.4f,0.3f,0.25f };

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	auto getTransform = [camera, &ecs]() {return &ecs.get_component<MEW::TransformComponent>(camera).value(); };
	auto getComponent = [&ecs]<typename T>(size_t entity) -> std::optional<T> {
		return ecs.get_component<T>(entity).value();
	};
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		
		
		if (w.isKeyPressed('W')) TS.TranslateZ(static_cast<float>(deltaTime) * 5,getTransform());
		if (w.isKeyPressed('A')) TS.TranslateX(static_cast<float>(deltaTime) * -5, getTransform());
		if (w.isKeyPressed('S')) TS.TranslateZ(static_cast<float>(deltaTime) * -5, getTransform());
		if (w.isKeyPressed('D')) TS.TranslateX(static_cast<float>(deltaTime) * 5, getTransform());
		if (w.isKeyPressed('Q')) TS.RotateX(5 * static_cast<float>(deltaTime), getTransform());
		if (w.isKeyPressed('E')) TS.RotateX(-5 * static_cast<float>(deltaTime), getTransform());
		if (w.isKeyPressed('Z')) TS.Scale(glm::vec3(1 * static_cast<float>(deltaTime)), getTransform());
		if (w.isKeyPressed('X')) TS.Scale(glm::vec3(-1 * static_cast<float>(deltaTime)), getTransform());
		printf("Camera position X: %f, Y: %f, Z %f\n",getTransform()->translation_.x, 
			getTransform()->translation_.y, 
			getTransform()->translation_.z);
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		csp(ecs.get_vectorComponent<MEW::TransformComponent>(), ecs.get_vectorComponent<MEW::CameraComponent>());
		csv(ecs.get_vectorComponent<MEW::TransformComponent>(), ecs.get_vectorComponent<MEW::CameraComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		ecs.get_component<MEW::CameraComponent>(camera);
		MEW::RenderSystemUnlit()(vecT,vecR,RS, shader, &ecs.get_component<MEW::CameraComponent>(camera));
		
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}
