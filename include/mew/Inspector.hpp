#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__ 1

#include "imgui.h"
#include "mew/Window.hpp"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_glfw.h"

namespace MEW {
  class Inspector {
  public:
    Inspector(MEW::Window);



    void WindowEntities();
    void HideAll();
    void ShowAll();
    void Render();

    ~Inspector();
  protected:
    template <typename T>
    void OpenWindow();
    ImGuiContext* context;
  private:
    //ImFontAtlas *atlas;
  };
}
#endif