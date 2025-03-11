#include "MEW/World.hpp"

namespace MEW {
  std::unique_ptr<World> World::instance = nullptr;

  World::World()
  {
    ecs_manager_ = nullptr;
  }

}
