#include "mew/Object.hpp"

#include "GL/glew.h"
#include <glm/gtc/matrix_transform.hpp>
/*
namespace MEW {
	Object::Object() {

	}
	Object::Object(std::string path, Shader* shader)
	{
		shader_ = shader;
		actMesh = 0;
		model = &Model(path.c_str());
	}

	Object::Object(Shader* shader)
	{
		shader_ = shader;
		actMesh = 0;
		//Load Meshes
		model = &Model();
	}

	Object::~Object()
	{
	}


	void Object::Draw()
	{
		shader_->UseProgram();
		glm::mat4 modelo = glm::mat4(1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);

		shader_->setMat4("model", modelo);
		shader_->setMat4("view", view);
		shader_->setMat4("projection", projection);
		model->Draw(*shader_);
	}

	void Object::UseProgram() {
		shader_->UseProgram();
	}
}
*/