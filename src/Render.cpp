#include <GL/glew.h>
#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#define CAMERA_WIDTH (1280)
#define CAMERA_HEIGHT (720)
namespace MEW {
  void GLAPIENTRY
    MessageCallback(GLenum source,
      GLenum type,
      GLuint id,
      GLenum severity,
      GLsizei length,
      const GLchar* message,
      const void* userParam)
  {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
      type, severity, message);
  }

	void RenderSystemUnlit::operator()operator()operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
		const std::vector<std::optional<MEW::RenderComponent>>& vecRender, MEW::Shader& shader,
		 CameraComponent* camComp) {


    shader.UseProgram();

		glm::mat4 view = camComp->viewMatrix;
		glm::mat4 projection = camComp->projectionMatrix;
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		auto itRender = vecRender.begin();
		auto itTransform = vecTransform.begin();
		for (; itTransform != vecTransform.end() && itRender != vecRender.end(); itTransform++, itRender++) {
			if (!itRender->has_value() || !itTransform->has_value()) continue;
			auto& render = itRender->value();
			auto& transform = itTransform->value();

      shader.setMat4("model", transform.model);
      if (!render.model->has_value()) continue;
      for (const auto& mesh : render.model->value().meshes_) {

        unsigned int textureUnit = 1;

        if (mesh.diffuse_tex_.has_value()) {
          glActiveTexture(GL_TEXTURE0 + textureUnit);
          shader.setInt("texture_diffuse1", textureUnit);
          glBindTexture(GL_TEXTURE_2D, mesh.diffuse_tex_.value().getID());
        }

        textureUnit++;

        if (mesh.normal_tex_.has_value()) {
          glActiveTexture(GL_TEXTURE0 + textureUnit);
          shader.setInt("texture_normal1", textureUnit);
          glBindTexture(GL_TEXTURE_2D, mesh.normal_tex_.value().getID());
        }

        textureUnit++;

        if (mesh.specular_tex_.has_value()) {
          glActiveTexture(GL_TEXTURE0 + textureUnit);
          shader.setInt("texture_specular1", textureUnit);
          glBindTexture(GL_TEXTURE_2D, mesh.specular_tex_.value().getID());
        }

        textureUnit++;

        glActiveTexture(GL_TEXTURE0);

        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // draw mesh
        glBindVertexArray(mesh.GetVAO());
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices_.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
      }
    }
  }

 
  
    void LightSystem::operator()(
      const std::vector<std::optional<MEW::TransformComponent>>&vecTrans,
      const std::vector<std::optional<MEW::RenderComponent>>&vecRender,
      std::vector<std::optional<MEW::LightComponent>>&vecLight,
      Shader & shader,
      std::optional<CameraComponent>&camComp) {
    }

    TransformComponent* ModelObject::GetTransformComponent() {
      return &ecs_->get_component<TransformComponent>(entity_).value();
    }

    RenderComponent* ModelObject::GetRenderComponent() {
      return &ecs_->get_component<RenderComponent>(entity_).value();
    }

    ModelObject::ModelObject(ECSManager& ecs) {
      ecs_ = &ecs;
      entity_ = ecs_->create_entity();
      ecs_->add_component<TransformComponent>(entity_);
      ecs_->add_component<RenderComponent>(entity_);
    }

    size_t ModelObject::GetEntity()
    {
      return entity_;
    }
}