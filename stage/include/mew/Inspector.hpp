#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__ 1

#include "imgui.h"
#include "mew/Window.hpp"
#include "ECSManager.hpp"
#include "mew/Input.hpp"
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_glfw.h"

namespace MEW {
  enum ActionsInspector
  {
    CLICK_OUT = 500,
  };

  //void Paint(const MEW::LightComponent &light);

  class Inspector {
  public:
    Inspector(MEW::Window &w);
    void update(float deltaTime, Input& inputManager);
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