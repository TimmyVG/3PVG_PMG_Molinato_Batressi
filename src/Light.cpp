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


namespace MEW {


  LightComponent::LightComponent(KTypeLight type ) : type(type) {
    direction = glm::vec3(0.0f);

    diffuse = glm::vec3(1.0f);
    fDiffuse = 1.0f;


    specular = glm::vec3(1.0f);
    fSpecular = 1.0f;

    shininess = 32.0f;
    constant = 1.0f;
    linear = 0.09;
    quadratic = 0.032f;

    cutOff = 12.5f;
    outerCutOff = 15.0f;
    bling = true;

    near_plane = 0.10f;
    far_plane = 100.5f;
    shadow_width = 1024;
    shadow_height = 1024;
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);

    if (type == KTypeLight::Point) {
      shadow_width = 2048;
      shadow_height = 2048;
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
      for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
          shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
      
    }
    else {


    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
      shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    }

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "Framebuffer error: " << status << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
