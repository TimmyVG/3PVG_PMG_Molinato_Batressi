#ifndef __WORLD_H__
#define __WORLD_H__ 1

#include <memory>

namespace MEW {

  class World
  {
  public:
    static World& GetWorld() {
      if (!instance) {
        instance = std::make_unique<World>();
      }
      return *instance;
    }

    static void ReleaseWorld() {
      instance.reset();
    }

    void setECSManager(class ECSManager* ecs_manager) {
      this->ecs_manager_ = ecs_manager;
    }

    class ECSManager* getECSManager() const {
      return ecs_manager_;
    }

    ~World() {};
  private:
    World();
    class ECSManager* ecs_manager_;

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    friend std::unique_ptr<World> std::make_unique<World>();
    static std::unique_ptr<World> instance;

  };


}

#endif