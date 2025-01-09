#include "mew/geometry.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace MEW {
  Geometry::Geometry(std::vector<float>& vertices, Shader* shader)
  {
    vertices_ = vertices;
    shader_ = shader;
    shader->CompileProgram(vertices_, &vao_);
    /*
    mat_ = glm::mat4(1.0f);
    scale_ = glm::vec3(1.0f);
    rotation_ = glm::vec3(0.0f);
    translation_ = glm::vec3(0.0f);
    */
  }
  void Geometry::DrawGeometry()
  {
    shader_->UseProgram();
    /*
    mat_ = glm::mat4x4(1.0f);
    mat_ = glm::scale(mat_, scale_);
    mat_ = glm::rotate(mat_, glm::radians(rotation_.x), glm::vec3(1.00f, 0.00f, 0.00f));
    mat_ = glm::rotate(mat_, glm::radians(rotation_.y), glm::vec3(0.00f, 1.00f, 0.00f));
    mat_ = glm::rotate(mat_, glm::radians(rotation_.z), glm::vec3(0.00f, 0.00f, 1.00f));
    mat_ = glm::translate(mat_, translation_);
    */

    //shader_->setMat4("transform", mat_);
    shader_->Draw(vao_);
  }
  void Geometry::UseProgram()
  {
    shader_->UseProgram();
  }
}

