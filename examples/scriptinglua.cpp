#include "mew/Window.hpp"
#include "mew/Scripting.hpp"
#include "GLFW/glfw3.h"
#include "lua.hpp"

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

	MEW::LuaScript scripting;
	std::string script = MEW::file_to_string("../data/scripts/helloworld.lua");

	scripting.add_global("multiplication", MEW::multiplication);
	scripting.run(script);
	while (!done) {
		w.newframe(backgroundcolor);


		bool closePressed = w.closedPressed();
		bool escPressed = false;
		if (closePressed || escPressed) done = true;
		w.endWindowFrame();

		//if (/*algo*/) done = true;
	}

	return 0;
}