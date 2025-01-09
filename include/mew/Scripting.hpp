#ifndef __SCRIPTING_H__
#define __SCRIPTING_H__ 1
#include "lua.hpp"
#include <memory>
#include <string>
#include <filesystem>
#include <fstream>
namespace MEW {
  struct ScriptingComponent {
    std::vector<std::string> scripts;
    std::unique_ptr<lua_State, decltype(&lua_close)> state_;
    lua_State* s;
    ScriptingComponent() : scripts(), state_(luaL_newstate(), &lua_close), s(state_.get()) {
      luaL_openlibs(s);
    };
    
  };

  class ScriptingSystem {
  public:
    void run(const ScriptingComponent& sc);
    void check(const ScriptingComponent& sc, int error);
    void add_global(const ScriptingComponent& sc, const std::string& name, int(*f)(lua_State*));
  };

  std::string file_to_string(const std::filesystem::path& path);
  int multiplication(lua_State* L);
}
#endif