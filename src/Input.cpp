#include "mew/Input.hpp"

namespace MEW {
	std::unordered_map<GLFWwindow*, Input*> Input::input_map;

	void Input::assign(Buttons k, int action)
	{
		current_frame_key_map.emplace(k, false);
		if (action_map.find(action) == action_map.end())
		{
			action_map.emplace(action, std::list<Buttons>{ k });
		}
		else
		{
			std::list<Buttons> key_list = action_map[action];
			key_list.push_back(k);
			action_map[action] = key_list;
		}
	}

	glm::vec2 Input::getMousePos()
	{
		return mousePos;
	}

	void Input::key_callback(int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_RELEASE:
			current_frame_key_map[(Buttons)key] = false;
			break;
		case GLFW_REPEAT:
			current_frame_key_map[(Buttons)key] = true;
		case GLFW_PRESS:
			current_frame_key_map[(Buttons)key] = true;
			break;
		}
	}

	void Input::mouse_button_callback(int button, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			current_frame_key_map[(Buttons)button] = true;
			break;
		case GLFW_RELEASE:
			current_frame_key_map[(Buttons)button] = false;
			break;
		}
	}


	void Input::global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		input_map[window]->key_callback(key, scancode, action, mods);
	}

	void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mousePos.x = static_cast<float>(xpos);
		mousePos.y = static_cast<float>(ypos);
	}

	void Input::global_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
	{
		input_map[window]->cursor_position_callback(window, xpos, ypos);
	}

	void Input::global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		input_map[window]->mouse_button_callback(button, action, mods);
	}

	Input::Input(GLFWwindow* window) : window_{ window } {
		input_map.emplace(window, this);
		glfwSetKeyCallback(window, global_key_callback);
		glfwSetCursorPosCallback(window, global_cursor_position_callback);
		glfwSetMouseButtonCallback(window, global_mouse_button_callback);
		mousePos = { 0.0,0.0 };
	}

	Input::~Input()
	{
		glfwSetKeyCallback(window_, nullptr);
		input_map.erase(window_);
	}
	void Input::newframe() {
		for (std::map<Buttons, bool>::iterator iter = current_frame_key_map.begin(); iter != current_frame_key_map.end(); ++iter) {
			changed_key_map[iter->first] = current_frame_key_map[iter->first] != last_frame_key_map[iter->first];
		}
		last_frame_key_map = current_frame_key_map;
	}
	bool Input::isKeyUp(int action)
	{
		std::list<Buttons> key = action_map[action];

		for (Buttons key : action_map[action]) {
			if (changed_key_map[key] && !current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyDown(int action) {
		std::list<Buttons> key = action_map[action];

		for (Buttons key : action_map[action]) {
			if (changed_key_map[key] && current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyPressed(int action)
	{
		std::list<Buttons> key = action_map[action];

		for (Buttons key : action_map[action]) {
			if (current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyReleased(int action)
	{
		std::list<Buttons> key = action_map[action];

		for (Buttons key : action_map[action]) {
			if (!current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}
}

