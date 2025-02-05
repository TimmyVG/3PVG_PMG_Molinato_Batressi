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