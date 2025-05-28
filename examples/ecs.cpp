#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include "mew/Render.hpp"
#include "mew/Model.hpp"
#include <iostream>
#include <cstdlib>  // Para rand() y srand()
#include <ctime> 


int global = 0;


int main() {
	srand(static_cast<unsigned>(time(0)));

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();

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

	MEW::Shader shader("../data/example.vs", "../data/example.fs");
	MEW::TextureData mikuDiffuseTextureData = MEW::TextureFromFile("texture_diffuse.png", "../data/miku/source");
	MEW::Texture mikuDiffuse(mikuDiffuseTextureData);
	std::optional<MEW::Model> TmpModel;
	std::vector<MEW::MeshData> TmpMeshData = MEW::loadModel("../data/miku/source/Miku.fbx").value();
	TmpModel = MEW::Model(TmpMeshData);
	const int rows = 10;
	const int cols = 10;
	const float spacing = 2.5f; // adjust as needed

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			MEW::ModelObject objmiku(ecs);
			*objmiku.GetRenderComponent()->model = TmpModel;

			auto& transform = *objmiku.GetTransformComponent();
			transform.scale_ = glm::vec3(1.0f);
			transform.rotation_ = glm::vec3(270.0f, 0.0f, 0.0f);

			// Position in a grid
			transform.translation_ = glm::vec3(j * spacing, 0.0f, -i * spacing);

			auto renderComponent = objmiku.GetRenderComponent();
			if (renderComponent && renderComponent->model->has_value()) {
				for (auto& mesh : renderComponent->model->value().meshes_) {
					mesh.diffuse_tex_ = mikuDiffuse;
				}
			}
		}
	}





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
	MEW::Camera cameraTest(ecs, 640 / 460);
	while (!done) {
		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);


		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		const auto& vecL = ecs.get_vectorComponent<MEW::LightComponent>();
		MEW::RenderSystemUnlit()(vecT, vecR, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}
	return 0;
}