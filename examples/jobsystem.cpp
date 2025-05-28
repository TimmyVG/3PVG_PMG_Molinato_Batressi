#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/Input.hpp"
#include "mew/JobSystem.hpp"
#include "mew/geometry.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Render.hpp"


enum Actions
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	CHANGE,
	CHANGE2,
};

int main() {

	MEW::ECSManager ecs;


	//AddComponents to ecs

	ecs.add_component_type<MEW::TransformComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
	ecs.add_component_type<MEW::RenderComponent>();

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

	

	const float color[3] = { 0.4f,0.3f,0.25f };
	MEW::Input input(w.window_);
	input.assign(MEW::Input::Buttons::MOUSE_1, CHANGE);

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;

	std::optional<MEW::Model> currentModel_;

	std::vector<std::string> obj_paths;
	obj_paths.push_back("../data/vivi/scene.gltf");
	obj_paths.push_back("../data/robot/scene.gltf");

	int objIndex = 0;
	std::future<std::optional<std::vector<MEW::MeshData>>> current_meshdata_future = js.add([objIndex, obj_paths]() { return MEW::loadModel(obj_paths[objIndex]); });
	objIndex++;

	MEW::ModelObject currentObject(ecs);
	currentObject.GetTransformComponent()->translation_ = glm::vec3(0,-1,-5);
	currentObject.GetTransformComponent()->rotation_ = glm::vec3(-90, 0, 0);

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
	bool loaded = false;
	while (!done) {
		input.newframe();

		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);

		if (current_meshdata_future.valid() && current_meshdata_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {

			std::vector<MEW::MeshData> current_meshData = current_meshdata_future.get().value();
			currentModel_ = MEW::Model(current_meshData);
			*currentObject.GetRenderComponent()->model = currentModel_;
			current_meshData.clear();
			
		}

		if (currentModel_) {

			if (input.isKeyDown(Actions::CHANGE)) {

				current_meshdata_future = js.add([&currentModel_, objIndex, obj_paths]() {
					std::optional<std::vector<MEW::MeshData>> next_meshData = MEW::loadModel(obj_paths[objIndex]);
					return next_meshData;
					});

				objIndex++;
				if (objIndex >= obj_paths.size()) objIndex = 0;
			}
		}
		
		const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
		const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
		MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
		MEW::RenderSystemUnlit()(vecT, vecR, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());

		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}