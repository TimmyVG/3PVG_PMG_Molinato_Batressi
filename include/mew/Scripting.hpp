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

    ScriptingComponent(ScriptingComponent&& other) noexcept
      : scripts(std::move(other.scripts)),
      state_(std::move(other.state_)),
      s(state_.get()) {
      other.s = nullptr;
    }

    ScriptingComponent& operator=(ScriptingComponent&& other) noexcept {
      if (this != &other) {
        scripts = std::move(other.scripts);
        state_ = std::move(other.state_);
        s = state_.get();
        other.s = nullptr;
      }
      return *this;
    }

    ScriptingComponent(const ScriptingComponent&) = delete;
    ScriptingComponent& operator=(const ScriptingComponent&) = delete;
  };

  class ScriptingSystem {
  public:
    void add_global(const std::vector<std::optional<ScriptingComponent>>& sc, const std::string& name, int(*f)(lua_State*));
    void operator()(const std::vector<std::optional<ScriptingComponent>>& scl);
  protected:
    void check(const ScriptingComponent& sc, int error);
    void run(const ScriptingComponent& sc);
  };

  std::string file_to_string(const std::filesystem::path& path);
  int multiplication(lua_State* L);
  int lua_get_position(lua_State* L);
  int lua_set_position(lua_State* L);
}
#endif