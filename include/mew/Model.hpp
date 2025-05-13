#ifndef __MODEL_H__
#define __MODEL_H__ 1
#include <vector>
#include <string>
#include <mew/Mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mew/Shader.hpp>
#include <unordered_map>

namespace MEW {
  class Model
  {
  public:

    Model(std::vector<MeshData>&);

    std::vector<Mesh> meshes_;

    void setUpData(std::vector<MeshData>& mesh_data);
  };


  static std::unordered_map<std::string, TextureData> tex_map;
  std::optional<std::vector<MeshData>> loadModel(std::string const& path);
  std::vector<MeshData> processNode(aiNode* node, const aiScene* scene, std::string directory_, std::vector<MeshData>& meshes_data_);
  MeshData processMesh(aiMesh* mesh, const aiScene* scene, std::string directory_);
  std::optional<TextureData> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory_);
}

#endif //__MODEL__