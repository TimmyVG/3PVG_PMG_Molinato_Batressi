#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__ 1

#include "imgui.h"
#include "mew/Window.hpp"
#include "ECSManager.hpp"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_glfw.h"

namespace MEW {
  class Inspector {
  public:
    Inspector(MEW::Window &w);

    //Need it to call WindowEntities
    void LinkECS(MEW::ECSManager &ecs);

    void NewFrame();

    void WindowEntities();
    void HideAll();
    void ShowAll();
    void Render();

    ~Inspector();
  protected:
    size_t EntityInspector;

    template<typename T>
    void TextComponent();
    bool InspectorEntitiesVisible;
    bool InspectorEntityVisible;
  private:
    ECSManager* ecs;
    //ImFontAtlas *atlas;
  };
}
#endif