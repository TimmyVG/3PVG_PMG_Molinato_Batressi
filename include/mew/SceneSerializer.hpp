#ifndef __SCENESERIALIZER_H__
#define __SCENESERIALIZER_H__ 1

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "Transform.hpp"
#include "ECSManager.hpp"
#include <iostream>

enum SaveActions {
  Save = 500000,
  Load,
  action2,
  action3
};

using json = nlohmann::json;

struct IdentityToJSON {
  size_t id;
  std::optional<MEW::TransformComponent> tr;
};

inline void to_json(json& j, const MEW::TransformComponent& t) {
  j = {
      {"position", {t.translation_.x, t.translation_.y, t.translation_.z}},
      {"rotation", {t.rotation_.x, t.rotation_.y, t.rotation_.z}},
      {"scale", {t.scale_.x, t.scale_.y, t.scale_.z}}
  };
}

inline void from_json(const json& j, MEW::TransformComponent& t) {
  auto pos = j.at("position");
  auto rot = j.at("rotation");
  auto scale = j.at("scale");

  t.translation_.x = pos[0];
  t.translation_.y = pos[1];
  t.translation_.z = pos[2];

  t.rotation_.x = rot[0];
  t.rotation_.y = rot[1];
  t.rotation_.z = rot[2];

  t.scale_.x = scale[0];
  t.scale_.y = scale[1];
  t.scale_.z = scale[2];
}

inline void to_json(json& j, const IdentityToJSON& id) {
  j["id"] = id.id;
  if (id.tr.has_value()) {
    json transformJson;
    to_json(transformJson, id.tr.value());
    j["transform"] = transformJson;
  }
}
 void SerializeScene(MEW::ECSManager& ecs) {
   std::vector<IdentityToJSON> identities;
   int num = 0;
   auto vectorTransform = ecs.get_vectorComponent<MEW::TransformComponent>();

   auto itTransform = vectorTransform.begin();
   for (; itTransform != vectorTransform.end(); itTransform++) {
     IdentityToJSON aux;
     aux.id = num;
     if (itTransform->has_value()) aux.tr = itTransform->value();


     //Add
     identities.push_back(aux);
     num++;
   }

   json j;
   j["entities"] = identities;

   std::filesystem::create_directories("../data/saves");
   std::ofstream out("../data/saves/scene.json");
   if (out.is_open()) {
     out << j.dump(4);
     out.close();
   }
   else {
     std::cerr << "Error serializing scene: " << std::endl;
   }
}

 inline void DeserializeScene(MEW::ECSManager& ecs) {
   std::ifstream in("../data/saves/scene.json");
   if (!in.is_open()) {
     std::cerr << "Error opening scene.json for deserialization" << std::endl;
     return;
   }

   json j;
   in >> j;

   auto& entities = j["entities"];
   for (const auto& entity : entities) {
     size_t id = entity.at("id");

     // Solo si tiene transform
     if (entity.contains("transform")) {
       MEW::TransformComponent tr;
       from_json(entity.at("transform"), tr);

       // Esto asume que el id es un índice en el vectorComponent
       auto& vector = ecs.get_vectorComponent<MEW::TransformComponent>();
       if (id < vector.size()) {
         if(vector[id].has_value() )vector[id] = tr;
       }
       else {
         std::cerr << "Warning: ID " << id << " fuera de rango" << std::endl;
       }
     }
   }
 }


#endif