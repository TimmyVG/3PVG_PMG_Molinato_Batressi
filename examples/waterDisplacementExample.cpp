#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include "mew/Model.hpp"
#include "mew/World.hpp"
#include "mew/Mesh.hpp"
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
	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
	ecs.add_component_type<MEW::RigidBodyComponent>();
	ecs.add_component_type<MEW::WaterComponent>();


	bool done = false;
	double deltaTime;

	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };


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
	MEW::Shader watershader("../data/displacementMap.vs", "../data/displacementMap.fs");
	MEW::TextureData waterdiffuseTextureData = MEW::TextureFromFile("WaterColor.jpg", "../data/water");
	MEW::TextureData waternormalTextureData = MEW::TextureFromFile("WaterNormal.jpg", "../data/Water");
	MEW::TextureData waterDisplacementTextureData = MEW::TextureFromFile("WaterDisp.png", "../data/Water");
	MEW::Texture waterdiffuseTexture(waterdiffuseTextureData);
	MEW::Texture waternormalTexture(waternormalTextureData);

	MEW::MeshData meshData = MEW::generateGridMesh(100,100,100,100);
	std::optional<MEW::Mesh> watermesh = std::make_optional<MEW::Mesh>(meshData);

	for (int x = 0; x < 4; ++x) {
		for (int z = 0; z < 4; ++z) {
			auto entityWater = ecs.create_entity();

			auto& transformwater = ecs.add_component<MEW::TransformComponent>(entityWater);
			transformwater.value().translation_ = glm::vec3(100 * x, 0, 100 * z);  

			auto& watercomponent = ecs.add_component<MEW::WaterComponent>(entityWater);
			*watercomponent.value().mesh = watermesh.value();
			watercomponent.value().mesh->value().diffuse_tex_ = waterdiffuseTexture;
			watercomponent.value().mesh->value().normal_tex_ = waterDisplacementTextureData;
		}
	}
	

#pragma endregion
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
	w.lastFrame_ = 0;
#pragma endregion
	float globalTime = 0.0f; // Initialize global time
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		if (deltaTime > 1.0) {
			deltaTime = 1.0f;
		}
		globalTime += deltaTime;
		cameraTest.update(deltaTime, input);
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		const auto& vecW = ecs.get_vectorComponent<MEW::WaterComponent>();
		printf("%f\n",globalTime);
		MEW::RenderSystemUnlit()(vecT, vecR, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());
		MEW::WaterRenderSystem()(vecT, vecW,watershader, ecs.get_component<MEW::CameraComponent>(cameraTest.entity_), globalTime);
		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}
	return 0;
}