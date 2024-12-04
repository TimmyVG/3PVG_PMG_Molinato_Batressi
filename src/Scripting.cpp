#include "mew/Scripting.hpp"
namespace MEW {
  void LuaScript::check(int error){
    if (error != LUA_OK) {
      std::string err = lua_tostring(s, lua_gettop(s));
      lua_pop(s, lua_gettop(s));
      throw std::runtime_error("Lua error:" + err);
    }
  }

  void LuaScript::run(const std::string& str){
    check(luaL_loadstring(s, str.c_str()));
    check(lua_pcall(s, 0, 0, 0));
    lua_pop(s, lua_gettop(s));
  }

  void LuaScript::add_global(const std::string& name, int(*function)(lua_State*)){
    lua_pushcfunction(s, function);
    lua_setglobal(s, name.c_str());
  }

  std::string file_to_string(const std::filesystem::path& path)
  {
    std::ifstream f(path.c_str());
    using str_iter = std::istreambuf_iterator<char>;
    std::string s{ str_iter{f}, str_iter{} };
    return s;
  }

  int multiplication(lua_State* L){
    int a = luaL_checkinteger(L, 1);
    int b = luaL_checkinteger(L, 2);
    lua_pushinteger(L, a * b);  // Return the product of a and b to Lua
    return 1;  // One return value on the stack
  }

}