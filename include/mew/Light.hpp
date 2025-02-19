#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <vector>
#include <optional>


namespace MEW {

  struct LightComponent {
    glm::vec3 color;
    glm::vec3 direction;
    unsigned int depthMap;
    unsigned int depthFBO;
    float near_plane = 1.0f, far_plane = 7.5;
    glm::mat4 lightProjection;

    LightComponent();
  };



}

#endif //__LIGHT__