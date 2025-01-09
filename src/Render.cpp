#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MEW {

	
	RenderComponent::RenderComponent()
	{
		object = std::make_shared<Object>();
	}
	
	void RenderSystem::Draw(RenderComponent* rc, TransformComponent* tc)
	{
		rc->object->UseProgram();
		glm::mat4 modelo = glm::mat4(1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);
		
		modelo = glm::translate(modelo,tc->translation_);
		modelo = glm::rotate(modelo, tc->rotation_.x, glm::vec3(1.0f,0.0f,0.0f));
		modelo = glm::rotate(modelo, tc->rotation_.y, glm::vec3(0.0f,1.0f,0.0f));
		modelo = glm::rotate(modelo, tc->rotation_.z, glm::vec3(0.0f,0.0f,1.0f));
		modelo = glm::scale(modelo, tc->scale_);

		

		rc->object->shader_->setMat4("model", modelo);
		rc->object->shader_->setMat4("view", view);
		rc->object->shader_->setMat4("projection", projection);
		rc->object->model->Draw(*rc->object->shader_);
	}

}