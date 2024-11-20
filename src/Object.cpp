#include "mew/Object.hpp"
#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>

namespace MEW {
	Object::Object(std::string path, Shader* shader)
	{
		shader_ = shader;
		actMesh = 0;
		//Load Meshes
		model = new Model(path.c_str());

		mat_ = glm::mat4(1.0f);
		scale_ = glm::vec3(1.0f);
		rotation_ = glm::vec3(0.0f);
		translation_ = glm::vec3(0.0f);
	}


	void Object::Draw()
	{
		shader_->UseProgram();
		glm::mat4 modelo = glm::mat4(1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);
		modelo = glm::translate(modelo,translation_);
		modelo = glm::rotate(modelo,rotation_.x, glm::vec3(1.0f,0.0f,0.0f));
		modelo = glm::rotate(modelo,rotation_.y, glm::vec3(0.0f,1.0f,0.0f));
		modelo = glm::rotate(modelo,rotation_.z, glm::vec3(0.0f,0.0f,1.0f));
		modelo = glm::scale(modelo, scale_);



		shader_->setMat4("model", modelo);
		shader_->setMat4("view", view);
		shader_->setMat4("projection", projection);
		model->Draw(*shader_);
	}

	void Object::UseProgram() {
		shader_->UseProgram();
	}

	void Object::SetTranslation(glm::vec3 pos)
	{
		translation_ = pos;
	}

	void Object::Translate(glm::vec3 pos) {
		translation_ += pos;
	}

	void Object::TranslateX(float pos)
	{
		translation_.x += pos;
	}

	void Object::TranslateY(float pos)
	{
		translation_.y += pos;
	}

	void Object::TranslateZ(float pos)
	{
		translation_.z += pos;
	}

	void Object::SetRotation(glm::vec3 rot)
	{
		rotation_ = rot;
	}

	void Object::Rotate(glm::vec3 rot) {
		rotation_ += rot;
	}

	void Object::RotateX(float rot) {
		rotation_.x += rot;
	}

	void Object::RotateY(float rot) {
		rotation_.y += rot;
	}

	void Object::RotateZ(float rot) {
		rotation_.z += rot;
	}

	void Object::SetScale(glm::vec3 scale)
	{
		scale_ = scale;
	}

	void Object::Scale(glm::vec3 scale) {
		scale_ += scale;
	}

	void Object::ScaleX(float scale) {
		scale_.x += scale;
	}

	void Object::ScaleY(float scale) {
		scale_.y += scale;
	}

	void Object::ScaleZ(float scale) {
		scale_.z += scale;
	}

	glm::vec3 Object::GetScale()
	{
		return glm::vec3(scale_);
	}

	glm::vec3 Object::GetRotation()
	{
		return glm::vec3(rotation_);
	}

	glm::vec3 Object::GetTranslation()
	{
		return glm::vec3(translation_);
	}

}
