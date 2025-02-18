#include <GL/glew.h>
#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)
#define CAMERA_WIDTH (640)
#define cAMERA_HEIGHT (460)
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

    modelo = glm::translate(modelo, tc->translation_);
    modelo = glm::rotate(modelo, tc->rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelo = glm::rotate(modelo, tc->rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelo = glm::rotate(modelo, tc->rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelo = glm::scale(modelo, tc->scale_);



		rc->object->shader_->setMat4("model", modelo);
		rc->object->shader_->setMat4("view", view);
		rc->object->shader_->setMat4("projection", projection);
		rc->object->model->Draw(*rc->object->shader_);
	}
	void RenderSystemUnlit::operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
		const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::RenderSystem& RS, MEW::Shader& shader,
		 std::optional<CameraComponent>* camComp) {


    shader.UseProgram();

		glm::mat4 view = camComp->value().viewMatrix;
		glm::mat4 projection = camComp->value().projectionMatrix;
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

  void RenderSystemLit::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    const std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader) {

    shader.UseProgram();
    glViewport(0, 0, CAMERA_WIDTH, cAMERA_HEIGHT);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    auto itT = vecTrans.begin();

    glBlendFunc(GL_ONE, GL_ZERO);

    auto itLight = vecLight.begin();
    for (; itLight != vecLight.end(); itLight++) {
      if (!itLight->has_value()) continue;
      glActiveTexture(GL_TEXTURE0 + 20);
      glBindTexture(GL_TEXTURE_2D, itLight->value().depthMap);
      shader.setInt("ShadowMap", 20);

      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();

        shader.setMat4("model", transform.model);
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
      glBlendFunc(GL_ONE, GL_ONE);

    }
  }
  void LightSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader) {

    for (auto& optionalLight : vecLight) {
      if (!optionalLight.has_value()) continue;
      auto& light = optionalLight.value();


      // Define light-space transformation (orthographic for directional light)

      //nose estoy probando
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, light.depthFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      shader.UseProgram();
      glEnable(GL_DEPTH_TEST);
      glBlendFunc(GL_ONE, GL_ZERO);
      glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f));
      glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 460.0f, 0.1f, 100.0f);


      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      // Render all objects to the depth map
      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();

        glActiveTexture(GL_TEXTURE0 + 20);
        glBindTexture(GL_TEXTURE_2D, light.depthMap);
        shader.setInt("ShadowMap", 20);
        shader.setMat4("model", transform.model);
        for (const auto& mesh : render.object->model->meshes) {
          glBindVertexArray(mesh.VAO);
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
      }
      GLenum state = glGetError(); 
      glClear(GL_DEPTH_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset to default framebuffer
    }
  }
}