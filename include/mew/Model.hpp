#ifndef __MODEL_H__
#define __MODEL_H__ 1
#include <vector>
#include <string>
#include <mew/Mesh.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <mew/Shader.hpp>

namespace MEW {
  class Model {
  public:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    bool gamaCorrection;

    Model();

    Model(const char* path);

    void Draw(Shader& shader);
    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
    bool loadModel(std::string path);
    void loadMeshes();
  private:
    //Model data

    

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
  };

}

#endif //__MODEL__