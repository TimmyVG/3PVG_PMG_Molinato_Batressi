#include "mew/Scripting.hpp"
namespace MEW {
  void ScriptingSystem::check(const ScriptingComponent& sc,int error){
    if (error != LUA_OK) {
      std::string err = lua_tostring(sc.s, lua_gettop(sc.s));
      lua_pop(sc.s, lua_gettop(sc.s));
      throw std::runtime_error("Lua error:" + err);
    }
  }

  void ScriptingSystem::run(const ScriptingComponent& sc){
    for (int i = 0; i < sc.scripts.size(); i++) {
      check(sc, luaL_loadstring(sc.s, sc.scripts[i].c_str()));
      check(sc, lua_pcall(sc.s, 0, 0, 0));
      lua_pop(sc.s, lua_gettop(sc.s));
    }
  }

  void ScriptingSystem::add_global(const ScriptingComponent& sc,const std::string& name, int(*function)(lua_State*)){
    lua_pushcfunction(sc.s, function);
    lua_setglobal(sc.s, name.c_str());
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

}