#include "mew/Object.hpp"
#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>
MEW::Object::Object(std::vector<float>& vertices, Shader* shader)
{
	vertices_ = vertices;
	shader_ = *shader;
	shader->CompileProgram(vertices_,&vao_);
	mat_ = glm::mat4(1.0f);
	scale_ = glm::vec3(1.0f);
	rotation_ = glm::vec3(0.0f);
	translation_ = glm::vec3(0.0f);
}


void MEW::Object::Draw()
{
	shader_.UseProgram();
	mat_ = glm::mat4x4(1.0f);
	mat_ = glm::scale(mat_, scale_);
	mat_ = glm::rotate(mat_, glm::radians(rotation_.x), glm::vec3(1.00f,0.00f,0.00f));
	mat_ = glm::rotate(mat_, glm::radians(rotation_.y), glm::vec3(0.00f,1.00f,0.00f));
	mat_ = glm::rotate(mat_, glm::radians(rotation_.z), glm::vec3(0.00f,0.00f,1.00f));
	mat_ = glm::translate(mat_, translation_);


	shader_.setMat4("transform",mat_);
	shader_.Draw(vao_);
}

void MEW::Object::UseProgram() {
	shader_.UseProgram();
}

void MEW::Object::SetTranslation(glm::vec3 pos)
{
	translation_ = pos;
}

void MEW::Object::Translate(glm::vec3 pos) {
	translation_ += pos;
}

void MEW::Object::TranslateX(float pos)
{
	translation_ = glm::vec3(translation_.x + pos, translation_.y, translation_.z);
}

void MEW::Object::TranslateY(float pos)
{
	translation_ = glm::vec3(translation_.x, translation_.y + pos, translation_.z);
}

void MEW::Object::TranslateZ(float pos)
{
	translation_ = glm::vec3(translation_.x, translation_.y, translation_.z + pos);
}

void MEW::Object::SetRotation(glm::vec3 rot)
{
	rotation_ = rot;
}

void MEW::Object::Rotate(glm::vec3 rot) {
	rotation_ += rot;
}

void MEW::Object::RotateX(float rot) {
	rotation_ = glm::vec3(rotation_.x + rot, rotation_.y, rotation_.z);
}

void MEW::Object::RotateY(float rot) {
	rotation_ = glm::vec3(rotation_.x, rotation_.y + rot, rotation_.z);
}

void MEW::Object::RotateZ(float rot) {
	rotation_ = glm::vec3(rotation_.x, rotation_.y, rotation_.z + rot);
}

void MEW::Object::SetScale(glm::vec3 scale)
{
	scale_ = scale;
}

void MEW::Object::Scale(glm::vec3 scale) {
	scale_ += scale;
}

void MEW::Object::ScaleX(float scale) {
	scale_ = glm::vec3(scale_.x + scale, scale_.y, scale_.z);
}

void MEW::Object::ScaleY(float scale) {
	scale_ = glm::vec3(scale_.x, scale_.y + scale, scale_.z);
}

void MEW::Object::ScaleZ(float scale) {
	scale_ = glm::vec3(scale_.x, scale_.y, scale_.z + scale);
}

