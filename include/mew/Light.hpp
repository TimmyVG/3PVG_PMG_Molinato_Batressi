#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include <vector>
#include <optional>


namespace MEW {

  struct LightComponent {
    glm::vec3 color;
    glm::vec3 direction;
    unsigned int depthMap;
    unsigned int depthFBO;
    LightComponent();
  };



}

#endif //__LIGHT__