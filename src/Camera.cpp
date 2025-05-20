#include <GL/glew.h>
#include "mew/Camera.hpp"
#include <gl/GL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

  MEW::Camera::Camera(MEW::ECSManager& ecsMan, float aspectRatio, CameraType type, float fov, float nearPlane, float farPlane, float orthosize, float zoom)
  {
    ecs = &ecsMan;
    entity_ = ecs->create_entity("Camera");
    ecs->add_component<CameraComponent>(entity_);
    ecs->add_component<TransformComponent>(entity_);
    CameraComponent* cameraComp = &ecs->get_component<CameraComponent>(entity_).value();
    TransformComponent* transformComp = &ecs->get_component<TransformComponent>(entity_).value();
    cameraComp->aspectRatio = aspectRatio;
    cameraComp->farPlane = farPlane;
    cameraComp->fov = fov;
    cameraComp->nearPlane = nearPlane;
    cameraComp->orthoSize = orthosize;
    cameraComp->type = type;
    cameraComp->zoom = zoom;
    calculateProjection();
    calculateView();

    glGenFramebuffers(1, &cameraComp->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, cameraComp->gBuffer);
    
    // - position color buffer
    glGenTextures(1, &cameraComp->gPosition);
    glBindTexture(GL_TEXTURE_2D, cameraComp->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1280, 720, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraComp->gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &cameraComp->gNormal);
    glBindTexture(GL_TEXTURE_2D, cameraComp->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1280, 720, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, cameraComp->gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &cameraComp->gColorSpec);
    glBindTexture(GL_TEXTURE_2D, cameraComp->gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, cameraComp->gColorSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    //4. Depth renderbuffer
      unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


    // 5. Check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    cameraComp->quadVAO = 0;
  }

  void MEW::Camera::update(float deltaTime, Input& inputManager)
  {
      CameraComponent* cameraComp = &ecs->get_component<CameraComponent>(entity_).value();
      TransformComponent* transformComp = &ecs->get_component<TransformComponent>(entity_).value();
      glm::vec3 moveDirection(0.0f);

      if (inputManager.isKeyPressed(CAMERA_FORWARD)) moveDirection += forward_;
      if (inputManager.isKeyPressed(CAMERA_BACK)) moveDirection -= forward_;
      if (inputManager.isKeyPressed(CAMERA_LEFT)) moveDirection -= glm::normalize(glm::cross(forward_, up_));
      if (inputManager.isKeyPressed(CAMERA_RIGHT)) moveDirection += glm::normalize(glm::cross(forward_, up_));
      //if (inputManager.isKeyPressed(KEY_Q)) moveDirection -= up_;
      //if (inputManager.isKeyPressed(KEY_E)) moveDirection += up_;

      if (glm::length(moveDirection) > 0) moveDirection = glm::normalize(moveDirection) * moveSpeed_ * deltaTime;
      if (inputManager.isKeyDown(CAMERA_ROTATE)) {
        glfwSetInputMode(inputManager.window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        inputManager.lastMousePos = inputManager.getMousePos();
      }
      glm::vec2 current_pos = inputManager.getMousePos();
      glm::vec2 delta_mouse = current_pos - inputManager.lastMousePos;
      inputManager.lastMousePos = current_pos;

      if (inputManager.isKeyPressed(CAMERA_ROTATE)) {
        transformComp->rotation_.x -= delta_mouse.y / inputManager.GetHeight() * lookSensitivity_;  // Pitch (invert Y)
        transformComp->rotation_.y += delta_mouse.x / inputManager.GetWidth() * lookSensitivity_;   // Yaw

        // Clamp pitch (X rotation)
        transformComp->rotation_.x = glm::clamp(transformComp->rotation_.x, -89.0f, 89.0f);
      }

      if (inputManager.getScrollOffset()!=0)
      {
        moveSpeed_ = std::max(0.1f, moveSpeed_ + inputManager.getScrollOffset());
        inputManager.SetScrollOffset(0.0f);
      }
      transformComp->translation_ += moveDirection;

     
      if (inputManager.isKeyUp(CAMERA_ROTATE))glfwSetInputMode(inputManager.window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      calculateView();
      calculateProjection();
  }

  void MEW::Camera::adjustSpeed(float offset)
  {
    moveSpeed_ = std::max(0.1f, moveSpeed_ + offset);
  }

  void MEW::Camera::calculateProjection()
  {
    CameraComponent* cameraComp = &ecs->get_component<CameraComponent>(entity_).value();
    if (cameraComp->type == CAMERA_PERSPECTIVE) {
      cameraComp->projectionMatrix = glm::perspective(
        glm::radians(cameraComp->fov), cameraComp->aspectRatio, cameraComp->nearPlane, cameraComp->farPlane
      );
    }
    else {
      float halfSize = cameraComp->orthoSize * 0.5f;
      cameraComp->projectionMatrix = glm::ortho(
        -halfSize * cameraComp->aspectRatio, halfSize * cameraComp->aspectRatio,
        -halfSize, halfSize, cameraComp->nearPlane, cameraComp->farPlane
      );
    }
  }
  
  MEW::CameraComponent* MEW::Camera::GetCameraComponent() {
    if (!ecs->get_component<CameraComponent>(entity_).has_value())
    {
      return nullptr;
    }
    return &ecs->get_component<CameraComponent>(entity_).value();
  }

  MEW::TransformComponent* MEW::Camera::GetTransformComp()
  {
    if (!ecs->get_component<TransformComponent>(entity_).has_value())
    {
      return nullptr;
    }
    return &ecs->get_component<TransformComponent>(entity_).value();
  }

  void MEW::Camera::calculateView()
  {
    CameraComponent* cameraComp = &ecs->get_component<CameraComponent>(entity_).value();
    TransformComponent* transformComp = &ecs->get_component<TransformComponent>(entity_).value();
    glm::vec3 position = transformComp->translation_;

    forward_ = glm::normalize(glm::vec3(
      sinf(glm::radians(transformComp->rotation_.y)) * cosf(glm::radians(transformComp->rotation_.x)),
      sinf(glm::radians(transformComp->rotation_.x)),
      -cosf(glm::radians(transformComp->rotation_.y)) * cosf(glm::radians(transformComp->rotation_.x))
    ));

    up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    right_ = glm::normalize(glm::cross(forward_, up_));
    up_ = glm::cross(right_, forward_);

    cameraComp->viewMatrix = glm::lookAt(position, position + forward_, up_);
  }
