#include <GL/glew.h>
#include "mew/Light.hpp"
#include <gl/GL.h>
#include "mew/Render.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
#include <glm/glm.hpp>
#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)

namespace MEW {


  LightComponent::LightComponent(KTypeLight type) : type(type) {
    color = glm::vec3(0.0f);
    depthMap = -1;
    depthFBO = -1;

    near_plane = 0.1f;
    far_plane = 100.0f;
    lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);

    glGenFramebuffers(1, &depthFBO);
    // Initialize depth texture if not set

    shinisses = 0.2f;
    diffuse_strenght = 0.2f;
    diffuse_color = glm::vec3(1.0f);
    spec_strength = 0.2f;
    spec_color = glm::vec3(1.0f);


    switch (type)
    {
    case MEW::None:
      break;
    case MEW::Directional:
      break;
    case MEW::Point:
      break;
    case MEW::Spot:
      cutoff = 1.0f;
      outercutoff = 1.0f;
      break;
    case MEW::Ambient:
      ambient_color = glm::vec3(1.0f);
      ambient_strength = 0.2f;
      break;
    default:
      break;
    }

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
      SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);


    glClear(GL_DEPTH_BUFFER_BIT);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer is not complete!" << std::endl;
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

  void UpdateLights::operator()(std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
                                std::vector<std::optional<MEW::LightComponent>>& vecLight, 
                                std::optional<TransformComponent> *transformCamera)
  {
    auto itLight = vecLight.begin();
    auto itTransformLight = vecTrans.begin();
    for (; itLight != vecLight.end(); itLight++, itTransformLight++) {
      if (!itLight->has_value()) continue;
      auto light = itLight->value();
      

      if (light.type == KTypeLight::Directional && transformCamera->has_value()) {
        if (!itTransformLight->has_value()) continue;
        auto transformLight = &itTransformLight->value();
/*
        transformLight->translation_ = transformCamera->value().translation_;
        glm::vec3 forward;
        auto CameraRotacion = transformCamera->value().rotation_;
        forward.x = cos(glm::radians(CameraRotacion.y)) * cos(glm::radians(CameraRotacion.x));
        forward.y = sin(glm::radians(CameraRotacion.x));
        forward.z = sin(glm::radians(CameraRotacion.y)) * cos(glm::radians(CameraRotacion.x));
        forward = glm::normalize(forward);
        transformLight->translation_.x -= forward.x * 20.0f;
        transformLight->translation_.z -= forward.z * 20.0f;
        transformLight->translation_.y += 20.0f;*/
      }
    }
  }

}
