#include "mew/Scripting.hpp"
#include "mew/World.hpp"
#include "mew/ECSManager.hpp"
#include "mew/Transform.hpp"
namespace MEW {
  void ScriptingSystem::check(const ScriptingComponent& sc,int error){
    if (error != LUA_OK) {
      std::string err = lua_tostring(sc.s, lua_gettop(sc.s));
      lua_pop(sc.s, lua_gettop(sc.s));
      throw std::runtime_error("Lua error:" + err);
    }
  }

  void ScriptingSystem::run(const ScriptingComponent& sc){
    for (const auto& it : sc.scripts) {
      check(sc, luaL_loadstring(sc.s, it.c_str()));
      check(sc, lua_pcall(sc.s, 0, 0, 0));
      lua_pop(sc.s, lua_gettop(sc.s));
    }
  }

  void ScriptingSystem::add_global(const std::vector<std::optional<ScriptingComponent>>& sc,const std::string& name, int(*function)(lua_State*)){
    auto it = sc.begin();
    for (;it != sc.end();it++)
    {
      if (!it->has_value())continue;
      auto& sc = it->value();
      lua_pushcfunction(sc.s, function);
      lua_setglobal(sc.s, name.c_str());
    }
  }

  void ScriptingSystem::operator()(const std::vector<std::optional<ScriptingComponent>>& scl) {
    auto it = scl.begin();
    for (;it != scl.end();it++)
    {
      if (!it->has_value())continue;
      auto& sc = it->value();
      for (const auto& its : sc.scripts) {
        check(sc, luaL_loadstring(sc.s, its.c_str()));
        check(sc, lua_pcall(sc.s, 0, 0, 0));
        lua_pop(sc.s, lua_gettop(sc.s));
      }
    }
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
    MEW::TransformComponent* transform = &ecs->get_component<TransformComponent>(luaL_checkinteger(L, 1)).value();
    transform->translation_.x = static_cast<float>(luaL_checknumber(L, 2));
    transform->translation_.y = static_cast<float>(luaL_checknumber(L, 3));
    transform->translation_.z = static_cast<float>(luaL_checknumber(L, 4));
    return 0; // No return values
  }
 

}