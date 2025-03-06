#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "mew/ECSManager.hpp"
#include <vector>
#include <optional>
#include "Camera.hpp"

namespace MEW {

  enum KTypeLight
  {
    None,
    Directional,
    Point,
    Spot,
    Ambient
  };

  struct LightComponent {
    glm::vec3 color;
    unsigned int depthMap;
    unsigned int depthFBO;
    float near_plane = -80.0f, far_plane = 40.0f;
    glm::mat4 lightProjection;
    KTypeLight type;
    float shinisses;
    float diffuse_strenght;
    glm::vec3 diffuse_color;
    float spec_strength;
    glm::vec3 spec_color;
    float cutoff;
    float outercutoff;
    float ambient_strength;
    glm::vec3 ambient_color;
    LightComponent(KTypeLight type);
  };



  class Light {
  public:
    size_t entity;
    Light(ECSManager& ecs, KTypeLight type);
  protected:
  private:
    ECSManager* ecs;
  };

  class UpdateLights {
  public:
    void operator()(std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
      std::vector<std::optional<MEW::LightComponent>>& vecLight,
      std::optional<TransformComponent> *camCompT);
  };
}

#endif //__LIGHT__