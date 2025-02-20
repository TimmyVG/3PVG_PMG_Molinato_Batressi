#ifndef __INPUT_H__
#define __INPUT_H__ 1
#include "GLFW/glfw3.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>
namespace MEW {
  class Camera;
  class Input{
    public:
      enum Buttons {
        MOUSE_1 = 0,
        MOUSE_2,
        MOUSE_3,
        KEY_SPACEBAR = 32,
        KEY_RIGHT = 262,
        KEY_LEFT,
        KEY_DOWN,
        KEY_UP,
        KEY_A = 65,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z = 90,
        Key_Count = 31
      };
      Input(GLFWwindow* window);
  
      ~Input();

      void newframe();

      bool isKeyUp(int action);
      bool isKeyDown(int action);
      bool isKeyPressed(int action);
      bool isKeyReleased(int action);

      void assign(Buttons k, int action);
      void SetMainCamera(Camera* cameratoset);
      void SetScrollOffset(float newoffset);
      glm::vec2 getMousePos();
      glm::vec2 getMouseDelta();
      glm::vec2 lastMousePos;
      float getScrollOffset();
      int GetWidth();
      int GetHeight();
      GLFWwindow* window_;
    private:
      void key_callback(int key, int scancode, int action, int mods);
      void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
      void mouse_button_callback(int button, int action, int mods);
      void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
      glm::vec2 mousePos;
      float scrollOffset_;
      std::map<int, std::list<Buttons>> action_map;
      std::map<Buttons, bool> current_frame_key_map;
      std::map<Buttons, bool> last_frame_key_map;
      std::map<Buttons, bool> changed_key_map;

      int width_, height_;
      static std::unordered_map<GLFWwindow*, Input*> input_map;
      static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
      static void global_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
      static void global_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
      static void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
      Camera* mainCamera_;
      GLFWgamepadstate state_;
  };

}


#endif