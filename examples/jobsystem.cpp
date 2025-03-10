#include "mew/Window.hpp"
#include "mew/Shader.hpp"
#include "mew/Object.hpp"
#include "mew/Input.hpp"
#include "mew/JobSystem.hpp"
#include "mew/geometry.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Render.hpp"
#include "mew/Model.hpp"

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
	ecs.add_component_type<MEW::RenderComponent>();
	ecs.add_component_type<MEW::CameraComponent>();
	MEW::RenderSystem RS;
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
	std::vector<float> pointvertex = {
 0.5f,  0.5f, 0.0f,  // top right
 0.5f, -0.5f, 0.0f,  // bottom right
-0.5f,  0.5f, 0.0f,  // top left 
	};



	std::optional<MEW::Model> currentModel_;
	bool rc_added = false;

	size_t entity = ecs.create_entity();
	ecs.add_component<MEW::TransformComponent>(entity);
	ecs.add_component<MEW::RenderComponent>(entity);

	const float color[3] = { 0.4f,0.3f,0.25f };
	MEW::Input input(w.window_);
	input.assign(MEW::Input::KEY_Q, Actions::CHANGE);
	input.assign(MEW::Input::KEY_E, Actions::CHANGE2);
	input.assign(MEW::Input::KEY_W, Actions::RIGHT);

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	std::string objdirectory = "../data/cube.obj";
	std::string objdirectorymiku = "../data/cube.obj";
	std::string objdirectorycube = "../data/cube.obj";
	std::vector<std::string> directories;
	directories.push_back(objdirectory);
	directories.push_back(objdirectorymiku);
	directories.push_back(objdirectorycube);
	int objindex = 0;
	auto current_meshdata_future = js.add([objindex, directories]() {
		return MEW::loadModel(directories[objindex]); });
	objindex++;
	MEW::Camera cameraTest(ecs, 640 / 460);
	ecs.get_component<MEW::TransformComponent>(cameraTest.entity_).value().translation_.z = 25;
	bool obj_loaded = false;
	bool aux_loaded = true;
	while (!done) {
		input.newframe();

		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();
		cameraTest.update(deltaTime, input);

		if (current_meshdata_future.valid() && current_meshdata_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			std::vector<MEW::MeshData> meshData = current_meshdata_future.get().value();
			currentModel_ = std::move(MEW::Model(meshData));
			*ecs.get_component<MEW::RenderComponent>(entity).value().model = currentModel_;
			if (!rc_added)
			{
				rc_added = true;
				ecs.add_component<MEW::RenderComponent>(entity);
			}
			meshData.clear();
		}
		if (input.isKeyPressed(Actions::CHANGE2))
		{
			ecs.get_component<MEW::TransformComponent>(entity).value().rotation_.x += 1.0f * deltaTime;
		}

		if (currentModel_) {
			MEW::TransformSystemMat()(ecs.get_vectorComponent<MEW::TransformComponent>());
			const auto& vecT = ecs.get_vectorComponent<MEW::TransformComponent>();
			const auto& vecR = ecs.get_vectorComponent<MEW::RenderComponent>();
			MEW::RenderSystemUnlit()(vecT, vecR, RS, shader, &ecs.get_component<MEW::CameraComponent>(cameraTest.entity_).value());
			if (input.isKeyDown(Actions::CHANGE)) {
				current_meshdata_future = js.add([&currentModel_, objindex, directories]() {
					std::optional<std::vector<MEW::MeshData>> meshData = MEW::loadModel(directories[objindex]);
					currentModel_ = std::nullopt;
					return meshData;
					});
				objindex++;
				if (objindex >= directories.size()) objindex = 0;

			}
		}


		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}