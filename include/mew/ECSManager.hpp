#include <optional>
#include <unordered_map>
#include <memory>
#include <cassert>
#include <any>
#include <type_traits>
#include "mew/Identity.hpp"
#include <typeindex>
#ifndef __ECSManager_H__
#define __ECSManager_H__ 1

namespace MEW {


struct ComponentListBase {
  virtual ~ComponentListBase() = default;
  virtual void grow(int optUntil = 0) = 0;
  virtual void Paint(size_t entidad) = 0;
  virtual size_t size() = 0;

};

template<typename T>
struct ComponentListDerived : ComponentListBase {
  std::vector<std::optional<T>> component_list_;

   void grow(int optUntil = 0) override {
     do {
       component_list_.resize(component_list_.size() + 1);
     } while (optUntil != 0 && component_list_.size() < optUntil);
   }

   void Paint(size_t entidad) override {

    // Paint(component_list_[entidad]);

   }

   size_t size() override {
     return component_list_.size();
   } 
};

class ECSManager {
public:
  ECSManager() {
    last_entity = 0;
    add_component_type<MEW::IdentityComponent>();
  }
  size_t create_entity() {
    size_t entity_id = last_entity++;
    for (auto& it: component_list_map) {
      it.second.get()->grow();
    }
    std::string defaultname = "Empty_";
    add_component<MEW::IdentityComponent>(entity_id, defaultname + std::to_string(entity_id));
    return entity_id;
  }

  size_t create_entity(std::string nameEntity) {
    size_t entity_id = last_entity++;
    for (auto& it : component_list_map) {
      it.second.get()->grow();
    }

    add_component<MEW::IdentityComponent>(entity_id, nameEntity);
    return entity_id;
  }


  template<typename T>
  void add_component_type() {
    size_t key = typeid(T).hash_code();
    component_list_map.emplace(key, std::make_unique<ComponentListDerived<T>>());
    map_type::iterator it = component_list_map.find(key);
    if (it != component_list_map.end()) it->second.get()->grow(static_cast<int>(last_entity));
  }

  template<typename T>
  std::optional<T>& get_component(size_t entity) {
    static std::optional<T> nullopt_val;
    size_t hash = typeid(T).hash_code();
    std::optional<T> component;
    map_type::iterator it = component_list_map.find(hash);
    if (it == component_list_map.end()) return nullopt_val;

    ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());

    return cld->component_list_[entity];
  }
 
  template<typename T> 
  std::optional<T>& add_component(size_t entity) {
    size_t hash = typeid(T).hash_code();
    std::optional<T> component;
    auto it = component_list_map.find(hash);
    if (it != component_list_map.end()) {
      ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
      if (cld->component_list_.at(entity).has_value()) {
        return cld->component_list_.at(entity);
      }
      else {
        cld->component_list_.at(entity) = T{};

        return cld->component_list_.at(entity);
      }
    }
    return component;
  }

  template<typename T, typename... Args> std::optional<T>& add_component(size_t entity, Args&&... args) {
    size_t key = typeid(T).hash_code();
    map_type::iterator it = component_list_map.find(key);
    if (it == component_list_map.end()) {
      static std::optional<T> null_opt = std::nullopt;
      return null_opt;
    }

    assert((entity < it->second->size()) && "ADD_COMPONENT::entity has an index greater than my map size.");

    ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
    cld->component_list_[entity].emplace(std::forward<Args>(args)...);

    return cld->component_list_[entity];
  }

  template<typename T>
  std::vector<std::optional<T>>& get_vectorComponent() {
    static std::vector<T> vec;
    size_t key = typeid(T).hash_code();
    component_list_map.emplace(key, std::make_unique<ComponentListDerived<T>>());
    map_type::iterator it = component_list_map.find(key);
 //   for (int i = 0; i < last_entity; ++i) {
 //     if (it->second.get() != std::nullopt_t)vec.push_back(it->second.get());
 //   }
    ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
    return cld->component_list_;

  }


  template<typename T>
  std::optional<T>& add_componentMove(size_t entity) {
    size_t hash = typeid(T).hash_code();
    static std::optional<T> component;
    auto it = component_list_map.find(hash);
    if (it != component_list_map.end()) {
      ComponentListDerived<T>* cld = static_cast<ComponentListDerived<T>*>(it->second.get());
      if (cld->component_list_.at(entity).has_value()) {
        component = std::move(cld->component_list_.at(entity));
      }
      else {
        cld->component_list_.at(entity) = T{};

        component = std::move(cld->component_list_.at(entity));
      }
    }
    return component;
  }
 
private:
  typedef std::unordered_map<size_t, std::unique_ptr<ComponentListBase>> map_type;
  map_type component_list_map;

  size_t last_entity;

};

}
#endif //__ECSManager__