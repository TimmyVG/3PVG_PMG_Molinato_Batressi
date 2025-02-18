#ifndef __CAMERA_H__
#define __CAMERA_H__ 1
#include <optional>
#include <vector>
#include "mew/Transform.hpp"
namespace MEW {
  typedef enum {
    CAMERA_ORTHOGRAPHIC,
    CAMERA_PERSPECTIVE
  } CameraType;

  typedef struct {
    CameraType type;
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    float orthoSize;
    glm::mat4 viewMatrix;       
    glm::mat4 projectionMatrix; 
  } CameraComponent;

  class CameraSystemProjection {
  public:
    void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
      std::vector<std::optional<CameraComponent>>& camComp);
  };

  class CameraSystemView {
  public:
    void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
      std::vector<std::optional<CameraComponent>>& camComp);
  };
}
#endif