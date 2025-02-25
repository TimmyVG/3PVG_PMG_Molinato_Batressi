#include <GL/glew.h>
#include "mew/Light.hpp"
#include <gl/GL.h>
#include "mew/Render.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)

namespace MEW {


  LightComponent::LightComponent() {
    color = glm::vec3(0.0f);
    direction = glm::vec3(0.0f);
    depthMap = -1;
    depthFBO = -1;

    near_plane = 0.1f;
    far_plane = 100.0f;
    lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);

    glGenFramebuffers(1, &depthFBO);
    // Initialize depth texture if not set


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
  Light::Light(MEW::ECSManager& ecs)
  {
    this->ecs = &ecs;

    type = KTypeLight::None;
    entity = ecs.create_entity();
    ecs.add_component<MEW::LightComponent>(entity);
    ecs.add_component<MEW::TransformComponent>(entity);
  }

  //Directional
  DirectionalLight::DirectionalLight(ECSManager& ecs) : Light(ecs)
  {
    type = KTypeLight::Directional;
  }

  PointLight::PointLight(ECSManager& ecs) : Light(ecs)
  {
    type = KTypeLight::Point;
  }

  SpotLight::SpotLight(ECSManager& ecs) : Light(ecs)
  {
    type = KTypeLight::Spot;
  }

  Ambient::Ambient(ECSManager& ecs) : Light(ecs)
  {
    type = KTypeLight::Ambient;
  }

}
