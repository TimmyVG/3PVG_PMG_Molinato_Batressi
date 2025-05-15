#ifndef __MESH_H__
#define __MESH_H__ 1
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <vector>
#include "Shader.hpp"

#define MAX_BONE_INFLUENCE 4

namespace MEW {

  struct Vertex {
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


  struct Texture {
    unsigned int id;
    std::string type;
    std::string path;

  };

  class Mesh {
  public:
    // mesh data
    std::vector<MEW::Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<MEW::Texture> textures_;
    unsigned int VAO;

    Mesh(std::vector<MEW::Vertex> vertices, std::vector<unsigned int> indices, std::vector<MEW::Texture> textures);
    void Draw(Shader& shader);

    void setupMesh();
  private:
    //  render data
    unsigned int VBO, EBO;
  };

}

#endif