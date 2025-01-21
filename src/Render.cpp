#include <GL/glew.h>
#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

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
	void RenderSystemUnlit::operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
		const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::RenderSystem& RS, MEW::Shader& shader) {

		//for (const auto& transform : vecTransform) {
		//for (std::vector<std::optional<MEW::TransformComponent>>::const_iterator itTransform = vecTransform.begin(); itTransform != vecTransform.end(); itTransform++) {
		//for (auto itTransform = vecTransform.begin(); itTransform != vecTransform.end(); itTransform++) {
		shader.UseProgram();

		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		auto itRender = vecRender.begin();
		auto itTransform = vecTransform.begin();
		for (; itTransform != vecTransform.end() && itRender != vecRender.end(); itTransform++, itRender++) {
			if (!itRender->has_value() || !itTransform->has_value()) continue;
			auto& render = itRender->value();
			auto& transform = itTransform->value();

			shader.setMat4("model", transform.model);

			//rc->object->model->Draw(*rc->object->shader_);
			//for (unsigned int i = 0; i < render.object->model->meshes.size(); i++)
			for (const auto& mesh : render.object->model->meshes)
			{
				// draw mesh
				unsigned int diffuseNr = 1;
				for (unsigned int j = 0; j < mesh.textures_.size(); j++) {
					glActiveTexture(GL_TEXTURE0 + j);
					std::string number;
					std::string name = mesh.textures_[j].type;
					//To simplificar
					if (name == "texture_diffuse") {
						number = std::to_string(diffuseNr++);
					}
					shader.setInt((name + number).c_str(), j);
					glBindTexture(GL_TEXTURE_2D, mesh.textures_[j].id);
				}
				glActiveTexture(GL_TEXTURE0);

				glDisable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);
				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}

	}
	void RenderSystemLit::operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
		const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
		MEW::Shader& shader)
	{

	}
}