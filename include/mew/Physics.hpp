#pragma once 
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <optional>
#include "Transform.hpp"
#include "Render.hpp"

#include <glm/gtc/quaternion.hpp>

namespace MEW {
  struct RigidBodyComponent{
    btRigidBody* body = nullptr;
    float mass = 0.0f;

    RigidBodyComponent() = default;

    RigidBodyComponent(btRigidBody* rb, float m)
      : body(rb), mass(m) {
    }
  };

  class PhysicsWorld {
  public:
    PhysicsWorld(ECSManager* ecs_);
    ~PhysicsWorld();

    void StepSimulation(float deltaTime);

    btRigidBody* AddRigidBody(float mass, const TransformComponent& transform, std::unique_ptr<btCollisionShape> shape);

    void SyncTransformFromRigidBody(btRigidBody* body, TransformComponent& transform);

    btRigidBody* AddBox(float mass, const TransformComponent& transform, const glm::vec3& halfExtents);

    void UpdateTransform(btRigidBody* body, TransformComponent& transform);

    btDiscreteDynamicsWorld* GetDynamicsWorld() { return m_dynamicsWorld.get(); }
  private:
    ECSManager* ecs_;
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btDbvtBroadphase> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

    std::vector<std::unique_ptr<btCollisionShape>> m_collisionShapes;
    std::vector<std::unique_ptr<btDefaultMotionState>> m_motionStates;
    std::vector<std::unique_ptr<btRigidBody>> m_rigidBodies;

  };

  
}
