#include "mew/Mesh.hpp"
#include "mew/Shader.hpp"
#include <GL/glew.h>
#include <iostream>

MEW::Mesh::Mesh(const MeshData& other_mesh_data) : indices_(other_mesh_data.ids_) {
  isMeshLoaded = false;
  setupMesh(other_mesh_data);
}

MEW::Mesh::Mesh(Mesh&& other) noexcept
  : indices_(std::move(other.indices_)), VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), isMeshLoaded(other.isMeshLoaded),
  textures_(std::move(other.textures_)) {
  other.VAO = other.VBO = other.EBO = 0;
}

MEW::Mesh& MEW::Mesh::operator=(const MEW::Mesh& other) {
  indices_ = other.indices_;
  textures_ = other.textures_;
  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  isMeshLoaded = other.isMeshLoaded;
  return *this;
}

MEW::Mesh::Mesh(const MEW::Mesh& other) {
  indices_ = other.indices_;
  textures_ = other.textures_;
  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  isMeshLoaded = other.isMeshLoaded;
}

MEW::Mesh::~Mesh()
{
  if (isMeshLoaded) {
    textures_.clear();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
}


void MEW::Mesh::setupMesh(const MeshData& data)
{
  if (isMeshLoaded) return;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, data.vertices_.size() * sizeof(VertexData), &data.vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(3 * sizeof(float)));
  // vertex uv
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(float)));

  glBindVertexArray(0);
  isMeshLoaded = true;
}
