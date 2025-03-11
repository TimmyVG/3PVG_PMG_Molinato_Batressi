#include "mew/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

  MEW::Camera::Camera(MEW::ECSManager& ecsMan, float aspectRatio, CameraType type, float fov, float nearPlane, float farPlane, float orthosize)
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
    calculateProjection();
    calculateView();
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
