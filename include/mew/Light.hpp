#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "mew/ECSManager.hpp"
#include <vector>
#include <optional>
#include "Camera.hpp"



namespace MEW {
  std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
  enum KTypeLight
  {
    None,
    Directional,
    Point,
    Spot,
    Ambient
  };

  struct LightComponent {
    KTypeLight type;
    glm::vec3 direction;

    glm::vec3 diffuse;
    float fDiffuse;

    glm::vec3 specular;
    float fSpecular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    float shininess;

    int bling;

    unsigned int depthMapFBO;
    unsigned int depthMap;
    //Shadows
    float* cameraFarPlane;
    unsigned int shadow_width, shadow_height;
    float near_plane, far_plane;
    std::vector<float> shadowCascadeLevels;
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
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
      const std::optional<TransformComponent>& transformCamera,
      const std::optional<CameraComponent>& cameraComponent);
  };
}

#endif //__LIGHT__