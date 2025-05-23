#include <mew/Physics.hpp>
#include <mew/ECSManager.hpp>

namespace MEW {

  PhysicsWorld::PhysicsWorld(ECSManager* ecs) {
    ecs_ = ecs;
    m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
    m_broadphase = std::make_unique<btDbvtBroadphase>();
    m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    m_dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
      m_dispatcher.get(),
      m_broadphase.get(),
      m_solver.get(),
      m_collisionConfig.get()
    );

    m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
  }

  PhysicsWorld::~PhysicsWorld()
  {
    for (auto& body : m_rigidBodies) {
      m_dynamicsWorld->removeRigidBody(body.get());  
    }

    m_rigidBodies.clear();  
    m_collisionShapes.clear();  
    m_motionStates.clear();  

    m_dynamicsWorld.reset();
    m_solver.reset();
    m_broadphase.reset();
    m_dispatcher.reset();
    m_collisionConfig.reset();
  }

  void PhysicsWorld::StepSimulation(float deltaTime) {
    if (m_dynamicsWorld) {
      m_dynamicsWorld->stepSimulation(1.0f/60,10);
      const std::vector<std::optional<MEW::RigidBodyComponent>>& vecRb = ecs_->get_vectorComponent<RigidBodyComponent>();
      std::vector<std::optional<MEW::TransformComponent>>& vecTrans = ecs_->get_vectorComponent<TransformComponent>();
      auto itRb = vecRb.begin();
      auto itRbTrans = vecTrans.begin();
      for (; itRb != vecRb.end() && itRbTrans != vecTrans.end(); itRb++, itRbTrans++) {
        if (!itRb->has_value()) continue;
        if (!itRbTrans->has_value()) continue;
        auto& rigidbodycomponent = itRb->value();
        auto& transforcomponent = itRbTrans->value();
        SyncTransformFromRigidBody(rigidbodycomponent.body, transforcomponent);
      }

    }
  }

  btRigidBody* PhysicsWorld::AddRigidBody(float mass, const TransformComponent& transform, std::unique_ptr<btCollisionShape> shape) {
    btCollisionShape* shapePtr = shape.get(); 

    m_collisionShapes.emplace_back(std::move(shape)); 

    btVector3 localInertia(0, 0, 0);
    if (mass != 0.f) {
      shapePtr->calculateLocalInertia(mass, localInertia);
    }

    btTransform btTrans;
    btTrans.setIdentity();
    btTrans.setOrigin(btVector3(
      transform.translation_.x,
      transform.translation_.y,
      transform.translation_.z
    ));

    btQuaternion quat;
    quat.setEulerZYX(
      transform.rotation_.z,
      transform.rotation_.y,
      transform.rotation_.x
    );
    btTrans.setRotation(quat);

    auto motionState = std::make_unique<btDefaultMotionState>(btTrans);
    auto* motionStatePtr = motionState.get();
    m_motionStates.push_back(std::move(motionState));

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionStatePtr, shapePtr, localInertia);
    auto rigidBody = std::make_unique<btRigidBody>(rbInfo);
    btRigidBody* bodyPtr = rigidBody.get();
    m_rigidBodies.push_back(std::move(rigidBody));

    m_dynamicsWorld->addRigidBody(bodyPtr);
    return bodyPtr;
  }

  void PhysicsWorld::SyncTransformFromRigidBody(btRigidBody* body, TransformComponent& transform) {
    if (body == nullptr) return;

 
    const btTransform& btTrans = body->getWorldTransform();
    const btVector3& position = btTrans.getOrigin();
    const btQuaternion& rotation = btTrans.getRotation();

    
    transform.translation_ = glm::vec3(position.x(), position.y(), position.z());
    transform.rotation_ = glm::vec3(rotation.getX(), rotation.getY(), rotation.getZ()); 
  }

  btRigidBody* PhysicsWorld::AddBox(float mass, const TransformComponent& transform, const glm::vec3& halfExtents) {
    auto shape = std::make_unique<btBoxShape>(btVector3(
      halfExtents.x * transform.scale_.x,
      halfExtents.y * transform.scale_.y,
      halfExtents.z * transform.scale_.z
    ));
    return AddRigidBody(mass, transform, std::move(shape));
  }

  void PhysicsWorld::UpdateTransform(btRigidBody* body, TransformComponent& transform) {
    if (body && body->getMotionState()) {
      btTransform btTrans;
      body->getMotionState()->getWorldTransform(btTrans);

      transform.translation_ = glm::vec3(
        btTrans.getOrigin().getX(),
        btTrans.getOrigin().getY(),
        btTrans.getOrigin().getZ()
      );

      btQuaternion rotation = btTrans.getRotation();
      glm::quat glmQuat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());
      transform.rotation_ = glm::eulerAngles(glmQuat);

    }
  }

} 