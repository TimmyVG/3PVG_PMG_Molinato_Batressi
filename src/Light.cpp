#include <GL/glew.h>
#include "mew/Light.hpp"
#include <gl/GL.h>
#include "mew/Render.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)

namespace MEW {


  LightComponent::LightComponent(KTypeLight type ) : type(type) {
    direction = glm::vec3(0.0f);

    diffuse = glm::vec3(1.0f);
    fDiffuse = 0.10f;


    specular = glm::vec3(1.0f);
    fSpecular = 0.10f;
    shininess = 1.0f;
    constant = 1.0f;
    linear = 0.09;
    quadratic = 0.032f;

    cutOff = 12.5f;
    outerCutOff = 15.0f;
  }

  //Light
  Light::Light(MEW::ECSManager& ecs, KTypeLight type) : ecs(&ecs)
  {

    entity = ecs.create_entity("Light");
    ecs.add_component<MEW::LightComponent>(entity,type);
    ecs.add_component<MEW::TransformComponent>(entity);
  }

  void UpdateLights::operator()(std::vector<std::optional<TransformComponent>>& vecTrans,
                                std::vector<std::optional<LightComponent>>& vecLight, 
                                const std::optional<TransformComponent>& transformCamera,
                                const std::optional<CameraComponent>& cameraComponent)
  {
    if (!transformCamera.has_value() || !cameraComponent.has_value()) return;
    auto trCamera = transformCamera.value();
    auto caCamera = cameraComponent.value();

    auto itTransform = vecTrans.begin();
    auto itLight = vecLight.begin();

    for (; itTransform != vecTrans.end() && itLight != vecLight.end(); itTransform++, itLight++) {
      if (!itTransform->has_value() || !itLight->has_value()) continue;
      auto trLight = itTransform->value();
      auto liLight = itLight->value();

      switch (liLight.type)
      {
      case KTypeLight::Directional:

        break;
      default:
        break;
      }
    }


  }

  std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
  {
    const auto inv = glm::inverse(proj * view);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
      for (unsigned int y = 0; y < 2; ++y)
      {
        for (unsigned int z = 0; z < 2; ++z)
        {
          const glm::vec4 pt =
            inv * glm::vec4(
              2.0f * x - 1.0f,
              2.0f * y - 1.0f,
              2.0f * z - 1.0f,
              1.0f);
          frustumCorners.push_back(pt / pt.w);
        }
      }
    }

    return frustumCorners;
  }
}
