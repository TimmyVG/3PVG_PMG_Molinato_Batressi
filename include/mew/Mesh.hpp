#ifndef __MESH_H__
#define __MESH_H__ 1
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "Shader.hpp"
#include "Texture.hpp"

#define MAX_BONE_INFLUENCE 4

namespace MEW {

  struct VertexData {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec2 Tangent;
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
  };

  struct MeshData {
    std::vector<VertexData> vertices_;
    std::vector<unsigned int> ids_;
    std::optional<std::vector<TextureData>> tex_data;
  };

  class Mesh {
  public:
    std::vector<unsigned int> indices_;
    std::vector<MEW::Texture> textures_;

    Mesh(const MeshData& other_mesh_data);
    ~Mesh();
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(const Mesh& other);
    Mesh(const Mesh& other);
    void setupMesh(const MeshData& data);
    unsigned int getVAO() { return VAO; };
  private:
    //  render data
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    bool isMeshLoaded;
  };

}

#endif