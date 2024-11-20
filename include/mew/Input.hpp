#ifndef __INPUT_H__
#define __INPUT_H__ 1
#include "GLFW/glfw3.h"
#include <map>
#include <unordered_map>
#include <vector>
namespace MEW {
  class Input{
    public:
      enum Keys {
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

      void assign(Keys k, int action);
      std::vector <double> getMousePos();
    private:
      void key_callback(int key, int scancode, int action, int mods);
      void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
      std::vector<double> mousePos;
      std::map<int, std::list<Keys>> action_map;
      std::map<Keys, bool> current_frame_key_map;
      std::map<Keys, bool> last_frame_key_map;
      std::map<Keys, bool> changed_key_map;

      GLFWwindow* window_;
      static std::unordered_map<GLFWwindow*, Input*> input_map;
      static void global_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
      static void global_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

  };

}


#endif