#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__ 1
#include "lua.hpp"
#include <memory>
#include <string>
#include <filesystem>
#include <fstream>


namespace MEW {
  std::string file_to_string(const std::filesystem::path& path);

  int multiplication(lua_State* L);
  class LuaScript {
  public:
    LuaScript() : state_{ luaL_newstate(), &lua_close }, s{state_.get()} {
      luaL_openlibs(s);
    }
    
    void check(int error);

    void run(const std::string& str);

    void add_global(const std::string& name, int(*function)(lua_State*));
  private:
    std::unique_ptr<lua_State, decltype(&lua_close)> state_;
    lua_State* s;
  };
}
#endif