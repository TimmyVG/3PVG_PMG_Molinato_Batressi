#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "mew/ECSManager.hpp"
#include <vector>
#include <optional>

namespace MEW {

  struct LightComponent {
    glm::vec3 color;
    glm::vec3 direction;
    unsigned int depthMap;
    unsigned int depthFBO;
    float near_plane = -80.0f, far_plane = 40.0f;
    glm::mat4 lightProjection;

    LightComponent();
  };

  enum KTypeLight
  {
    None,
    Directional,
    Point,
    Spot,
    Ambient
  };

  class Light {
  public:
    size_t entity;
    Light(ECSManager &ecs);
  protected:
    KTypeLight type;
  private:
    ECSManager* ecs;
  };

  class DirectionalLight : Light {
  public:
    DirectionalLight(ECSManager& ecs);
  };

  class PointLight : Light {
  public:

    PointLight(ECSManager& ecs);
  };

  class SpotLight : Light {
  public:

    SpotLight(ECSManager& ecs);
  };

  class Ambient : Light {
  public:

    Ambient(ECSManager& ecs);
  };
}

#endif //__LIGHT__