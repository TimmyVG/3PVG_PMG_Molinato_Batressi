#include "GLFW/glfw3.h"
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
	ecs.add_component_type<MEW::RenderComponent>();
	MEW::TransformSystem TS;
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
	MEW::Shader shaderTriangle("../data/triangle.vs", "../data/triangle.fs");
	std::vector<float> pointvertex = {
 0.5f,  0.5f, 0.0f,  // top right
 0.5f, -0.5f, 0.0f,  // bottom right
-0.5f,  0.5f, 0.0f,  // top left 
	};


	MEW::Object obj2(&shader);
	MEW::Object objaux(&shader);

	size_t entity = ecs.create_entity();
	ecs.add_component<MEW::TransformComponent>(entity);
	ecs.add_component<MEW::RenderComponent>(entity);
	MEW::RenderComponent* rc = &ecs.get_component<MEW::RenderComponent>(entity).value();
	MEW::TransformComponent* tc = &ecs.get_component<MEW::TransformComponent>(entity).value();

	MEW::Geometry triangle(pointvertex, &shaderTriangle);
	const float color[3] = { 0.4f,0.3f,0.25f };
	MEW::Input input(w.window_);
	input.assign(MEW::Input::Buttons::MOUSE_1, CHANGE);
	input.assign(MEW::Input::Buttons::MOUSE_2, CHANGE2);
	//Triangle Movement
	input.assign(MEW::Input::Buttons::KEY_A, LEFT);
	input.assign(MEW::Input::Buttons::KEY_LEFT, LEFT);
	input.assign(MEW::Input::Buttons::KEY_D, RIGHT);
	input.assign(MEW::Input::Buttons::KEY_RIGHT, RIGHT);
	input.assign(MEW::Input::Buttons::KEY_W, UP);
	input.assign(MEW::Input::Buttons::KEY_UP, UP);
	input.assign(MEW::Input::Buttons::KEY_S, DOWN);
	input.assign(MEW::Input::Buttons::KEY_DOWN, DOWN);
	TS.TranslateZ(-10,tc);
	TS.TranslateY(-3, tc);

	TS.RotateX(-45.0f, tc);

	TS.SetScale(glm::vec3(1.0f), tc);

	bool done = false;
	const float backgroundcolor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	double deltaTime;
	std::string objdirectory = "../data/Silla.fbx";
	std::string objdirectorymiku = "../data/miku/source/Miku.fbx";
	std::string objdirectorycube = "../data/spiderman.fbx";
	std::vector<std::string> directories;
	directories.push_back(objdirectory);
	directories.push_back(objdirectorymiku);
	directories.push_back(objdirectorycube);
	int objindex = 0;
	auto pruebaFuture = js.add([&objaux,objindex,directories]() {
		 return objaux.model->loadModel(directories[objindex]);
		});
	objindex++;

	bool obj_loaded = false;
	bool aux_loaded = true;
	while (!done) {
		input.newframe();

		w.newframe(backgroundcolor);
		deltaTime = w.deltaTime();

		obj2.UseProgram();

		if (pruebaFuture.valid() && pruebaFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			obj_loaded = pruebaFuture.get();
			obj2.model = objaux.model;
			obj2.model->loadMeshes();
			MEW::RenderComponent *auxrc =  &ecs.get_component<MEW::RenderComponent>(entity).value();
			*auxrc->object = std::move(obj2);
			aux_loaded = false;
		}
		if (input.isKeyPressed(UP)) triangle.TranslateY(static_cast<float>(deltaTime) * 1.0f);
		if (input.isKeyPressed(LEFT)) triangle.TranslateX(static_cast<float>(deltaTime) * -1.0f);
		if (input.isKeyPressed(DOWN)) triangle.TranslateY(static_cast<float>(deltaTime) * -1.0f);
		if (input.isKeyPressed(RIGHT)) triangle.TranslateX(static_cast<float>(deltaTime) * 1.0f);
		if (obj_loaded&&!aux_loaded)
		{
			RS.Draw(&ecs.get_component<MEW::RenderComponent>(entity).value(), &ecs.get_component<MEW::TransformComponent>(entity).value());
			if (input.isKeyUp(CHANGE))
			{
				if (objindex!=2)
				{
					TS.SetScale(glm::vec3(1.0f), &ecs.get_component<MEW::TransformComponent>(entity).value());
				}
				else
				{
					TS.SetScale(glm::vec3(0.1f), &ecs.get_component<MEW::TransformComponent>(entity).value());
				}
				objaux.model->meshes.clear();
				objaux.model->textures_loaded.clear();
				pruebaFuture = js.add([&objaux, objindex, directories]() {
					return objaux.model->loadModel(directories[objindex]);
					});
				aux_loaded = true;
				objindex++;
				if (objindex >= directories.size())
				{
					objindex = 0;
				}
			}
		}
		triangle.Draw();


		bool closePressed = w.closedPressed();
		bool escPressed = w.isKeyPressed(GLFW_KEY_ESCAPE);
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();
	}


	return 0;
}