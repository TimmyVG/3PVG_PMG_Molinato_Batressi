#include "mew/Window.hpp"
#include "mew/Scripting.hpp"
#include "GLFW/glfw3.h"
#include "lua.hpp"
#include "mew/ECSManager.hpp"

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
	#pragma endregion

	int nEntity = 13;
	MEW::ECSManager ecs;
	std::vector<size_t> entities;
	MEW::ScriptingSystem SS;
	MEW::ScriptingComponent SC;
	std::string script = MEW::file_to_string("../data/scripts/helloworld.lua");
	std::string script2 = MEW::file_to_string("../data/scripts/holamundo.lua");

	ecs.add_component_type<MEW::ScriptingComponent>();
	for (int i = 0; i < nEntity; i++) {
		size_t entity = ecs.create_entity();
		SC.scripts.push_back(script);
		SC.scripts.push_back(script2);
		ecs.add_componentMove<MEW::ScriptingComponent>(entity);
		MEW::ScriptingComponent* ScriptComp = &ecs.get_component<MEW::ScriptingComponent>(entity).value();
		*ScriptComp = std::move(SC);

		SS.add_global(ecs.get_component<MEW::ScriptingComponent>(entity).value(), "multiplication", MEW::multiplication);
		entities.push_back(entity);
	}

	for (int i = 0; i < nEntity; i++)
	{
		SS.run(ecs.get_component<MEW::ScriptingComponent>(entities.at(i)).value());
	}
	while (!done) {
		w.newframe(backgroundcolor);


		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();

	}

	return 0;
}