#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__ 1

#include "ImgUtil.h"
#include "mew/Window.hpp"

namespace MEW {
  class Inspector {
  public:
    Inspector();



    void WindowEntities();
    void HideAll();
    void ShowAll();
    void Render();
   // ImGuiIO& io;
  protected:
    template <typename T>
    void OpenWindow();

  private:
    //ImFontAtlas *atlas;
  };
}
#endif