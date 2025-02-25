#ifndef __INSPECTOR_H__
#define __INSPECTOR_H__ 1

#include "ImgUtil.h"

namespace MEW {
  class Inspector {
  public:
    Inspector();



    void WindowEntities();
    void HideAll();
    void ShowAll();

  protected:

    template <typename T>
    void OpenWindow();

  };
}
#endif