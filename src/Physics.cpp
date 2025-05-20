#include <mew/Physics.hpp>

namespace MEW {

  PhysicsWorld::PhysicsWorld() {
    // Bullet initialization (smart pointers handle allocation/deletion)
    m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
    m_broadphase = std::make_unique<btDbvtBroadphase>();
    m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    // Create the dynamics world
    m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
      m_dispatcher.get(),
      m_broadphase.get(),
      m_solver.get(),
      m_collisionConfig.get()
    );

    // Optional: Set gravity (default is (0, 0, 0))
    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
  }

  void PhysicsWorld::StepSimulation(float deltaTime) {
    if (m_dynamicsWorld) {
      m_dynamicsWorld->stepSimulation(1.0f/60,10);
      for (int j = m_dynamicsWorld->getNumCollisionObjects()-1; j >= 0; j--)
      {
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
        btRigidBody* body = btRigidBody::upcast(obj);
        btTransform trans;
        if (body && body->getMotionState())
        {
          body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
          trans = obj->getWorldTransform();
        }
        printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
      }
    }
  }

  btRigidBody* PhysicsWorld::AddRigidBody(float mass, const TransformComponent& transform, btCollisionShape* shape) {
    // Store the shape
    m_collisionShapes.emplace_back(shape);

    // Calculate local inertia
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f) {
      shape->calculateLocalInertia(mass, localInertia);
    }

    // Convert TransformComponent to btTransform
    btTransform btTrans;
    btTrans.setIdentity();

    // Use the translation from your component
    btTrans.setOrigin(btVector3(
      transform.translation_.x,
      transform.translation_.y,
      transform.translation_.z
    ));

    // Convert Euler angles (in radians) to quaternion
    btQuaternion quat;
    quat.setEulerZYX(
      transform.rotation_.z, // yaw (Z)
      transform.rotation_.y, // pitch (Y)
      transform.rotation_.x  // roll (X)
    );
    btTrans.setRotation(quat);

    // Create motion state
    auto motionState = new btDefaultMotionState(btTrans);

    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    auto body = new btRigidBody(rbInfo);

    // Add to world
    m_dynamicsWorld->addRigidBody(body);

    return body;
  }

  btRigidBody* PhysicsWorld::AddBox(float mass, const TransformComponent& transform, const glm::vec3& halfExtents) {
    // Note: Bullet's box shape uses half extents
    auto shape = new btBoxShape(btVector3(
      halfExtents.x * transform.scale_.x,
      halfExtents.y * transform.scale_.y,
      halfExtents.z * transform.scale_.z
    ));
    return AddRigidBody(mass, transform, shape);
  }

  void PhysicsWorld::UpdateTransform(btRigidBody* body, TransformComponent& transform) {
    if (body && body->getMotionState()) {
      btTransform btTrans;
      body->getMotionState()->getWorldTransform(btTrans);

      // Update translation
      transform.translation_ = glm::vec3(
        btTrans.getOrigin().getX(),
        btTrans.getOrigin().getY(),
        btTrans.getOrigin().getZ()
      );

      // Update rotation (convert quaternion to Euler angles)
      btQuaternion rotation = btTrans.getRotation();
      glm::quat glmQuat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
      transform.rotation_ = glm::eulerAngles(glmQuat);

      // Rebuild matrices if needed
    }
  }

} 