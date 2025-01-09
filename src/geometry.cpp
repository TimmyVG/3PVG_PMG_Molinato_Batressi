#include "mew/geometry.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace MEW {


	Geometry::Geometry(std::vector<float>& vertices, Shader* shader)
	{
		vertices_ = vertices;
		shader_ = shader;
		shader->CompileProgram(vertices_, &vao_);
		mat_ = glm::mat4(1.0f);
		scale_ = glm::vec3(1.0f);
		rotation_ = glm::vec3(0.0f);
		translation_ = glm::vec3(0.0f);
	}


	void Geometry::Draw()
	{
		shader_->UseProgram();
		mat_ = glm::mat4x4(1.0f);
		mat_ = glm::scale(mat_, scale_);
		mat_ = glm::rotate(mat_, glm::radians(rotation_.x), glm::vec3(1.00f, 0.00f, 0.00f));
		mat_ = glm::rotate(mat_, glm::radians(rotation_.y), glm::vec3(0.00f, 1.00f, 0.00f));
		mat_ = glm::rotate(mat_, glm::radians(rotation_.z), glm::vec3(0.00f, 0.00f, 1.00f));
		mat_ = glm::translate(mat_, translation_);


		shader_->setMat4("transform", mat_);
		shader_->Draw(vao_);
	}

	void Geometry::UseProgram() {
		shader_->UseProgram();
	}

	void Geometry::SetTranslation(glm::vec3 pos)
	{
		translation_ = pos;
	}

	void Geometry::Translate(glm::vec3 pos) {
		translation_ += pos;
	}

	void Geometry::TranslateX(float pos)
	{
		translation_.x += pos;
	}

	void Geometry::TranslateY(float pos)
	{
		translation_.y += pos;
	}

	void Geometry::TranslateZ(float pos)
	{
		translation_.z += pos;
	}

	void Geometry::SetRotation(glm::vec3 rot)
	{
		rotation_ = rot;
	}

	void Geometry::Rotate(glm::vec3 rot) {
		rotation_ += rot;
	}

	void Geometry::RotateX(float rot) {
		rotation_.x += rot;
	}

	void Geometry::RotateY(float rot) {
		rotation_.y += rot;
	}

	void Geometry::RotateZ(float rot) {
		rotation_.z += rot;
	}

	void Geometry::SetScale(glm::vec3 scale)
	{
		scale_ = scale;
	}

	void Geometry::Scale(glm::vec3 scale) {
		scale_ += scale;
	}

	void Geometry::ScaleX(float scale) {
		scale_.x += scale;
	}

	void Geometry::ScaleY(float scale) {
		scale_.y += scale;
	}

	void Geometry::ScaleZ(float scale) {
		scale_.z += scale;
	}

	glm::vec3 Geometry::GetScale()
	{
		return glm::vec3(scale_);
	}

	glm::vec3 Geometry::GetRotation()
	{
		return glm::vec3(rotation_);
	}

	glm::vec3 Geometry::GetTranslation()
	{
		return glm::vec3(translation_);
	}
}

