#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include "mew/Model.hpp"
#include "mew/World.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime>
#include "mew/Physics.hpp"
// Standard C++
#include <vector>      // For std::vector
#include <memory>      // For smart pointers (if using)
#include <glm/glm.hpp> // For glm::mat4/vec3 (if doing rendering)
#include <glm/gtc/type_ptr.hpp> // For glm::value_ptr


int main() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;
#pragma region PhysicsFunctions
	MEW::PhysicsWorld physicsWorld;
	// Create a static ground plane
	MEW::TransformComponent groundTransform;
	groundTransform.translation_ = glm::vec3(0, -1, 0);
	groundTransform.scale_ = glm::vec3(50, 1, 50);
	btRigidBody* groundBody = physicsWorld.AddBox(0.0f, groundTransform, glm::vec3(1, 1, 1));

	// Create a dynamic box
	MEW::TransformComponent boxTransform;
	boxTransform.translation_ = glm::vec3(0, 10, -10);
	btRigidBody* boxBody = physicsWorld.AddBox(1.0f, boxTransform, glm::vec3(1, 1, 1));
#pragma endregion
	//AddComponents to ecs
	bool done = false;
	double deltaTime;

	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	
	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
#pragma region WindowCreation

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
#pragma endregion
#pragma region OBJcreation
	MEW::Shader shader("../data/example.vs", "../data/example.fs");

	std::optional<MEW::Model> TmpModel;
	std::vector<MEW::MeshData> TmpMeshData = MEW::loadModel("../data/miku/source/miku.fbx").value();
	TmpModel = MEW::Model(TmpMeshData);
	MEW::ModelObject objmiku(ecs);
	*objmiku.GetRenderComponent()->model = TmpModel;
	objmiku.GetTransformComponent()->scale_ = glm::vec3(1, 1, 1);
	objmiku.GetTransformComponent()->rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	objmiku.GetTransformComponent()->translation_ = glm::vec3(0.0f, 0.0f, 0.0f);
#pragma endregion
	MEW::Shader debugShader("../data/debug.vs", "../data/debug.fs");
#pragma region Camera
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
#pragma endregion

	MEW::PhysicsRenderSystem renderSystem;
	renderSystem.SetShaderAndCamera(&debugShader, cameraTest.GetCameraComponent());
	physicsWorld.GetDynamicsWorld()->setDebugDrawer(&renderSystem);

	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);
		physicsWorld.StepSimulation(deltaTime);
		physicsWorld.UpdateTransform(boxBody, boxTransform);
		*objmiku.GetTransformComponent() = boxTransform;
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		MEW::RenderSystemUnlit()(vecT, vecR, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());
		physicsWorld.GetDynamicsWorld()->debugDrawWorld();
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}
	return 0;
}