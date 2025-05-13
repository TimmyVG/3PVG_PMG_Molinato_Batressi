#include "mew/Scripting.hpp"
#include "mew/World.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
#include <iostream>
namespace MEW {
  void ScriptingSystem::check(const ScriptingComponent& sc, int error) {
    if (error != LUA_OK) {
      int top = lua_gettop(sc.s);
      std::string err = (top > 0) ? lua_tostring(sc.s, -1) : "Unknown Lua error";
      lua_pop(sc.s, (top > 0) ? 1 : 0); 
      throw std::runtime_error("Lua error: " + err);
    }
  }

  void ScriptingSystem::add_global(const std::vector<std::optional<ScriptingComponent>>& sc,const std::string& name, int(*function)(lua_State*)){
    auto it = sc.begin();
    for (;it != sc.end();it++)
    {
      if (!it->has_value())continue;
      auto& scr = it->value();
      lua_pushcfunction(scr.s, function);
      lua_setglobal(scr.s, name.c_str());
    }
  }

  void ScriptingSystem::operator()(const std::vector<std::optional<ScriptingComponent>>& scl) {
    auto it = scl.begin();
    for (; it != scl.end(); it++) {
      if (!it->has_value()) continue;
      auto& sc = it->value();
      for (const auto& script : sc.scripts) {
        int initialStackSize = lua_gettop(sc.s);  // Save the stack size before loading the script

        printf("[RUN] Lua stack before loading: %d\n", initialStackSize);
        check(sc, luaL_loadstring(sc.s, script.c_str()));
        printf("[RUN] Lua stack before execution: %d\n", lua_gettop(sc.s));

        check(sc, lua_pcall(sc.s, 0, 0, 0));

        int afterExecutionStackSize = lua_gettop(sc.s); // Check stack size after execution
        printf("[RUN] Lua stack after execution: %d\n", afterExecutionStackSize);

        // If returnCount (stack growth) > 0, ensure to pop the stack correctly.
        int returnCount = afterExecutionStackSize - initialStackSize;
        if (returnCount > 0) {
          lua_pop(sc.s, returnCount);  // Only pop the additional values pushed by pcall
        }

        // Debugging: Ensure the stack is consistent after each execution
        int finalStackSize = lua_gettop(sc.s);
        if (finalStackSize != initialStackSize) {
          printf("[RUN] Stack size mismatch! Expected: %d, Actual: %d\n", initialStackSize, finalStackSize);
        }
      }
    }
  }

  void ScriptingSystem::register_functions(const std::vector<std::optional<ScriptingComponent>>& scl)
  {
    add_global(scl, "GetPosition", MEW::lua_get_position);
    add_global(scl, "SetPosition", MEW::lua_set_position);
    add_global(scl, "multiplication", MEW::multiplication);
    add_global(scl, "CreateEntity", MEW::lua_create_entity);
  }

  std::string file_to_string(const std::filesystem::path& path)
  {
    std::ifstream f(path.c_str());
    using str_iter = std::istreambuf_iterator<char>;
    std::string s{ str_iter{f}, str_iter{} };
    return s;
  }

  int multiplication(lua_State* L){
    int a = static_cast<int>(luaL_checkinteger(L, 1));
    int b = static_cast<int>(luaL_checkinteger(L, 2));
    lua_pushinteger(L, a * b);  // Return the product of a and b to Lua
    return 1;  // One return value on the stack
  }
  
  int lua_get_position(lua_State* L) {
    ECSManager* ecs = World::GetWorld().getECSManager();
    if (!ecs->get_component<TransformComponent>(luaL_checkinteger(L, 1)).has_value())
    {
      lua_pushnumber(L, -1);
      lua_pushnumber(L, -1);
      lua_pushnumber(L, -1);
      return 3;
    }

    MEW::TransformComponent* transform = &ecs->get_component<TransformComponent>(luaL_checkinteger(L,1)).value();
    lua_pushnumber(L, transform->translation_.x);
    lua_pushnumber(L, transform->translation_.y);
    lua_pushnumber(L, transform->translation_.z);
    return 3; // Returning x, y, and z
  }

  int lua_set_position(lua_State* L) {
    ECSManager* ecs = World::GetWorld().getECSManager();
    if (!ecs->get_component<TransformComponent>(luaL_checkinteger(L, 1)).has_value()) {
      return 0;
    }
    MEW::TransformComponent* transform = &ecs->get_component<TransformComponent>(luaL_checkinteger(L, 1)).value();
    transform->translation_.x = static_cast<float>(luaL_checknumber(L, 2));
    transform->translation_.y = static_cast<float>(luaL_checknumber(L, 3));
    transform->translation_.z = static_cast<float>(luaL_checknumber(L, 4));
    return 0; // No return values
  }
  int lua_create_entity(lua_State* L) {
    ECSManager* ecs = World::GetWorld().getECSManager();
    size_t entity = ecs->create_entity();
    lua_pushinteger(L, entity);
    return 1; // Returning entity number
  }

  int lua_destroy_entity(lua_State* l) {
    ECSManager* ecs = World::GetWorld().getECSManager();
    //size_t entity = ecs->destroy_entity(luaL_checkinteger(L, 1));
    return 0; 
  }

}