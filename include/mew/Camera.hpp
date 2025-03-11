#ifndef __CAMERA_H__
#define __CAMERA_H__ 1
#include <optional>
#include <vector>
#include "mew/Transform.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Input.hpp"
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

  enum Actions
  {
    CAMERA_LEFT = 333,
    CAMERA_RIGHT,
    CAMERA_FORWARD,
    CAMERA_BACK,
    CAMERA_ROTATE
  };

  class Camera
  {
  public:
    Camera(MEW::ECSManager& ecs,
            float aspectRatio,
            CameraType type = CAMERA_PERSPECTIVE, 
            float fov = 50.0f, 
            float nearPlane = 0.01f, 
            float farPlane = 1000.0f, 
            float orthosize = 10.0f);
    ~Camera() {};

    void update(float deltaTime, Input& inputManager);
    void adjustSpeed(float offset);
    CameraComponent* GetCameraComponent();
    TransformComponent* GetTransformComp();
    size_t entity_;
  private:
    ECSManager* ecs;

    glm::vec3 forward_;
    glm::vec3 up_;
    glm::vec3 right_;

    float moveSpeed_ = 5.0f;
    float lookSensitivity_ = 100.0f;
    float yaw_ = -90.0f;
    float pitch_ = 0.0f;

    void calculateProjection();
    void calculateView();
  };
}
#endif