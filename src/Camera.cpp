#include "mew/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

void MEW::CameraSystemProjection::operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform, std::vector<std::optional<CameraComponent>>& camComp)
{
  auto itTransform = vecTransform.begin();
  auto itCamera = camComp.begin();

  for (; itTransform != vecTransform.end() && itCamera != camComp.end(); ++itTransform, ++itCamera) {
    if (!itTransform->has_value() || !itCamera->has_value()) continue;

    auto& camera = itCamera->value();
    auto& transform = itTransform->value();

    if (camera.type == CAMERA_PERSPECTIVE) {
      camera.projectionMatrix = glm::perspective(
        glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane
      );
    }
    else {
      float halfSize = camera.orthoSize * 0.5f;
      camera.projectionMatrix = glm::ortho(
        -halfSize * camera.aspectRatio, halfSize * camera.aspectRatio,
        -halfSize, halfSize, camera.nearPlane, camera.farPlane
      );
    }

  }
}


  void MEW::CameraSystemView::operator()(const std::vector<std::optional<MEW::TransformComponent>>&vecTransform, std::vector<std::optional<CameraComponent>>&camComp)
  {
    auto itTransform = vecTransform.begin();
    auto itCamera = camComp.begin();

    for (; itTransform != vecTransform.end() && itCamera != camComp.end(); ++itTransform, ++itCamera) {
      if (!itTransform->has_value() || !itCamera->has_value()) continue;

      auto& camera = itCamera->value();
      auto& transform = itTransform->value();

      glm::vec3 position = transform.translation_;

      glm::vec3 forward = glm::normalize(glm::vec3(
        sinf(glm::radians(transform.rotation_.y)) * cosf(glm::radians(transform.rotation_.x)),
        sinf(glm::radians(transform.rotation_.x)),
        -cosf(glm::radians(transform.rotation_.y)) * cosf(glm::radians(transform.rotation_.x))
      ));

      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
      glm::vec3 right = glm::normalize(glm::cross(forward, up));
      up = glm::cross(right, forward);

      camera.viewMatrix = glm::lookAt(position, position + forward, up);
    }
  }
  
  MEW::Camera::Camera(MEW::ECSManager& ecs, float aspectRatio, CameraType type, float fov, float nearPlane, float farPlane, float orthosize)
  {
    entity_ = ecs.create_entity();
    ecs.add_component<CameraComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    cameraComp = &ecs.get_component<CameraComponent>(entity_).value();
    transformComp = &ecs.get_component<TransformComponent>(entity_).value();
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
      glm::vec3 moveDirection(0.0f);

      if (inputManager.isKeyPressed(CAMERA_FORWARD)) moveDirection += forward_;
      if (inputManager.isKeyPressed(CAMERA_BACK)) moveDirection -= forward_;
      if (inputManager.isKeyPressed(CAMERA_LEFT)) moveDirection -= glm::normalize(glm::cross(forward_, up_));
      if (inputManager.isKeyPressed(CAMERA_RIGHT)) moveDirection += glm::normalize(glm::cross(forward_, up_));
      //if (inputManager.isKeyPressed(KEY_Q)) moveDirection -= up_;
      //if (inputManager.isKeyPressed(KEY_E)) moveDirection += up_;

      if (glm::length(moveDirection) > 0) moveDirection = glm::normalize(moveDirection) * moveSpeed_ * deltaTime;
      if (inputManager.isKeyDown(CAMERA_ROTATE)) inputManager.lastMousePos = inputManager.getMousePos();
      glm::vec2 current_pos = inputManager.getMousePos();
      glm::vec2 delta_mouse = current_pos - inputManager.lastMousePos;
      inputManager.lastMousePos = current_pos;

      if (inputManager.isKeyPressed(CAMERA_ROTATE)) {
        transformComp->rotation_.x -= delta_mouse.y / inputManager.GetHeight() * lookSensitivity_;  // Pitch (invert Y)
        transformComp->rotation_.y += delta_mouse.x / inputManager.GetWidth() * lookSensitivity_;   // Yaw

        // Clamp pitch (X rotation)
        transformComp->rotation_.x = glm::clamp(transformComp->rotation_.x, -89.0f, 89.0f);
      }

      transformComp->translation_ += moveDirection;

     
     
      calculateView();
  }

  void MEW::Camera::calculateProjection()
  {
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

  void MEW::Camera::calculateView()
  {
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
