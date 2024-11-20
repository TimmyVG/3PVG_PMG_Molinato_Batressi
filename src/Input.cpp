#include "mew/Input.hpp"

namespace MEW {
	std::unordered_map<GLFWwindow*, Input*> Input::input_map;

	void Input::assign(Keys k, int action)
	{
		current_frame_key_map.emplace(k, false);
		if (action_map.find(action) == action_map.end())
		{
			action_map.emplace(action, std::list<Keys>{ k });
		}
		else
		{
			std::list<Keys> key_list = action_map[action];
			key_list.push_back(k);
			action_map[action] = key_list;
		}
	}

	std::vector<double> Input::getMousePos()
	{
		return mousePos;
	}

	void Input::key_callback(int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_RELEASE:
			current_frame_key_map[(Keys)key] = false;
			break;
		case GLFW_REPEAT:
			current_frame_key_map[(Keys)key] = true;
		case GLFW_PRESS:
			current_frame_key_map[(Keys)key] = true;
			break;
		}
	}

	void Input::global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		input_map[window]->key_callback(key, scancode, action, mods);
	}

	void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mousePos[0] = xpos;
		mousePos[1] = ypos;
	}

	void Input::global_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
	{
		input_map[window]->cursor_position_callback(window, xpos, ypos);
	}

	Input::Input(GLFWwindow* window) : window_{ window } {
		input_map.emplace(window, this);
		glfwSetKeyCallback(window, global_key_callback);
		glfwSetCursorPosCallback(window, global_cursor_position_callback);
		mousePos = { 0.0,0.0 };
	}

	Input::~Input()
	{
		glfwSetKeyCallback(window_, nullptr);
		input_map.erase(window_);
	}
	void Input::newframe() {
		for (std::map<Keys, bool>::iterator iter = current_frame_key_map.begin(); iter != current_frame_key_map.end(); ++iter) {
			changed_key_map[iter->first] = current_frame_key_map[iter->first] != last_frame_key_map[iter->first];
		}
		last_frame_key_map = current_frame_key_map;
	}
	bool Input::isKeyUp(int action)
	{
		std::list<Keys> key = action_map[action];

		for (Keys key : action_map[action]) {
			if (changed_key_map[key] && !current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyDown(int action) {
		std::list<Keys> key = action_map[action];

		for (Keys key : action_map[action]) {
			if (changed_key_map[key] && current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyPressed(int action)
	{
		std::list<Keys> key = action_map[action];

		for (Keys key : action_map[action]) {
			if (current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}

	bool Input::isKeyReleased(int action)
	{
		std::list<Keys> key = action_map[action];

		for (Keys key : action_map[action]) {
			if (!current_frame_key_map[key]) {
				return true;
			}
		}
		return false;
	}
}

