#pragma once 
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <optional>
#include "Transform.hpp"
#include "Render.hpp"

#include <glm/gtc/quaternion.hpp>

namespace MEW {
  class PhysicsWorld {
  public:
    PhysicsWorld();
    ~PhysicsWorld() {};

    void StepSimulation(float deltaTime);

    // Add a rigid body using TransformComponent
    btRigidBody* AddRigidBody(float mass, const TransformComponent& transform, btCollisionShape* shape);

    // Helper to create a simple box
    btRigidBody* AddBox(float mass, const TransformComponent& transform, const glm::vec3& halfExtents);

    // Update transform from physics to your component
    void UpdateTransform(btRigidBody* body, TransformComponent& transform);

    // Get the dynamics world for more advanced operations
    btDiscreteDynamicsWorld* GetDynamicsWorld() { return m_dynamicsWorld.get(); }
  private:
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btDbvtBroadphase> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

    std::vector<std::unique_ptr<btCollisionShape>> m_collisionShapes;
  };

  
}
