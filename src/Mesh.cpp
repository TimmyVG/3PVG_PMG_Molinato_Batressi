#include "mew/Mesh.hpp"
#include "mew/Shader.hpp"
#include <GL/glew.h>
#include <iostream>
MEW::Mesh::Mesh(const MeshData& other_mesh_data) : indices_(other_mesh_data.ids_) {
  isMeshLoaded = false;
  setUpMesh(other_mesh_data);
}

MEW::Mesh::~Mesh() {
  if (isMeshLoaded) {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
}

MEW::Mesh::Mesh(Mesh&& other) noexcept
  : indices_(std::move(other.indices_)), VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), isMeshLoaded(other.isMeshLoaded) {
  other.VAO = other.VBO = other.EBO = 0;

  if (other.diffuse_tex_.has_value()) diffuse_tex_ = std::move(other.diffuse_tex_.value());
  else diffuse_tex_ = std::nullopt;

  if (other.normal_tex_.has_value()) normal_tex_ = std::move(other.normal_tex_.value());
  else normal_tex_ = std::nullopt;

  if (other.specular_tex_.has_value()) specular_tex_ = std::move(other.specular_tex_.value());
  else specular_tex_ = std::nullopt;

}

MEW::Mesh& MEW::Mesh::operator=(Mesh& other) {
  indices_ = other.indices_;
  if (other.diffuse_tex_.has_value()) diffuse_tex_ = other.diffuse_tex_.value();
  else diffuse_tex_ = std::nullopt;

  if (other.normal_tex_.has_value()) normal_tex_ = other.normal_tex_.value();
  else normal_tex_ = std::nullopt;

  if (other.specular_tex_.has_value()) specular_tex_ = other.specular_tex_.value();
  else specular_tex_ = std::nullopt;

  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  isMeshLoaded = other.isMeshLoaded;
  return *this;
}

MEW::Mesh::Mesh(Mesh& other) {
  indices_ = other.indices_;
  if (other.diffuse_tex_.has_value()) diffuse_tex_ = other.diffuse_tex_.value();
  else diffuse_tex_ = std::nullopt;

  if (other.normal_tex_.has_value()) normal_tex_ = other.normal_tex_.value();
  else normal_tex_ = std::nullopt;

  if (other.specular_tex_.has_value()) specular_tex_ = other.specular_tex_.value();
  else specular_tex_ = std::nullopt;
  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  isMeshLoaded = other.isMeshLoaded;
}

void MEW::Mesh::setUpMesh(const MeshData& mesh_data) {
  if (isMeshLoaded) return;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, mesh_data.vertexs_.size() * sizeof(Vertex_Data), &mesh_data.vertexs_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Data), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Data), (void*)(3 * sizeof(float)));
  // vertex uv
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Data), (void*)(6 * sizeof(float)));

  glBindVertexArray(0);
  isMeshLoaded = true;
}

const unsigned int MEW::Mesh::GetVAO() const
{
  return VAO;
}
namespace MEW {
  MeshData generateGridMesh(int width, int height, int resolutionX, int resolutionY)
  {
    MEW::MeshData meshData;

    float dx = static_cast<float>(width) / (resolutionX - 1);
    float dz = static_cast<float>(height) / (resolutionY - 1);

    for (int y = 0; y < resolutionY; ++y) {
      for (int x = 0; x < resolutionX; ++x) {
        float u = static_cast<float>(x) / (resolutionX - 1);
        float v = static_cast<float>(y) / (resolutionY - 1);

        MEW::Vertex_Data vertex;
        vertex.position = glm::vec3(x * dx, 0.0f, y * dz);
        vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertex.uv = glm::vec2(u, v);

        meshData.vertexs_.push_back(vertex);
      }
    }

    for (int y = 0; y < resolutionY - 1; ++y) {
      for (int x = 0; x < resolutionX - 1; ++x) {
        int i = y * resolutionX + x;

        meshData.ids_.push_back(i);
        meshData.ids_.push_back(i + resolutionX);
        meshData.ids_.push_back(i + 1);

        meshData.ids_.push_back(i + 1);
        meshData.ids_.push_back(i + resolutionX);
        meshData.ids_.push_back(i + resolutionX + 1);
      }
    }

    return meshData;
  }
}

