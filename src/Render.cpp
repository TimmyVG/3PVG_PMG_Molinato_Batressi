#include <GL/glew.h>
#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)
#define CAMERA_WIDTH (1280)
#define cAMERA_HEIGHT (720)
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
    Shader& shader,
    std::optional<CameraComponent>* camComp,
    std::optional<TransformComponent>* camCompT) {

    shader.UseProgram();

    glViewport(0, 0, CAMERA_WIDTH, cAMERA_HEIGHT);
    glm::mat4 view = camComp->value().viewMatrix;
    glm::mat4 projection = camComp->value().projectionMatrix;

    shader.setMat4("u_view_projection", projection * view);
    shader.setFloat3("u_camera_pos", &camCompT->value().translation_.x);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    auto itT = vecTrans.begin();

    glBlendFunc(GL_ONE, GL_ZERO);

    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++ , itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto lightT = itLightT->value();
      auto lightL = itLight->value();
      //Bind depthMap
      glActiveTexture(GL_TEXTURE0 + 10);
      glBindTexture(GL_TEXTURE_2D, itLight->value().depthMap);

      //Light space transforms
      glm::vec3 forward = glm::normalize(glm::vec3(
        sinf(glm::radians(lightT.rotation_.y)) * cosf(glm::radians(lightT.rotation_.x)),
        sinf(glm::radians(lightT.rotation_.x)),
        -cosf(glm::radians(lightT.rotation_.y)) * cosf(glm::radians(lightT.rotation_.x))
      ));
      glm::mat4 lightView = glm::lookAt(lightT.translation_, lightT.translation_ + forward, glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 lightSpaceMatrix = lightL.lightProjection * lightView;

      //Uniforms
      shader.setInt("u_type", itLight->value().type);
      shader.setMat4("u_lightSpaceMatrix", lightSpaceMatrix);
      shader.setInt("u_shadowMap", 10);
      shader.setFloat3("u_lightPos", &itLightT->value().translation_.x);
      glm::vec3 euler  = glm::radians(itLightT->value().rotation_);
      glm::quat rotation = glm::quat(euler);
      glm::vec3 forwardD(0.0f, 0.0f, 1.0f);
      glm::vec3 direction = rotation * forwardD;
      direction = glm::normalize(direction);
      shader.setFloat3("u_light_dir", &direction.x);
      shader.setFloat3("u_viewPos", &camCompT->value().translation_.x);
      shader.setFloat("u_shininess", itLight->value().shinisses);
      shader.setFloat("u_diffuse_strength", itLight->value().diffuse_strenght);
      shader.setFloat3("u_diffuse_color", &itLight->value().diffuse_color.x);
      shader.setFloat("u_spec_strength", itLight->value().spec_strength);
      shader.setFloat3("u_spec_color", &itLight->value().spec_color.x);
      shader.setFloat("u_cutoff", itLight->value().cutoff);
      shader.setFloat("u_outercutoff", itLight->value().outercutoff);
      shader.setFloat("u_ambient_strength", itLight->value().ambient_strength);
      shader.setFloat3("u_ambient_color", &itLight->value().ambient_color.x);


      //shader.setMat4("lightSpaceMatrix", itLightT->value().)
      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();

        shader.setMat4("u_model", transform.model);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform.model)));
        shader.setMat4("u_normalMatrix", normalMatrix);


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

          if (mesh.textures_.size() == 0) {
            glBindTexture(GL_TEXTURE_2D, 0);

          }

          glActiveTexture(GL_TEXTURE0);


          glBindVertexArray(mesh.VAO);
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
      }
      glBlendFunc(GL_ONE, GL_ONE);
      glCullFace(GL_BACK);

    }
  }
  void LightSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader,
    std::optional<CameraComponent>* camComp) {

    shader.UseProgram();
    glCullFace(GL_FRONT);

    glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);
    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto light = itLight->value();
      auto lightT = itLightT->value();

      glBindFramebuffer(GL_FRAMEBUFFER, light.depthFBO);
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glClear(GL_DEPTH_BUFFER_BIT);

      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glBlendFunc(GL_ONE, GL_ZERO);
      glDepthFunc(GL_LEQUAL);


      glm::mat4 view = camComp->value().viewMatrix;
      glm::mat4 projection = camComp->value().projectionMatrix;

      shader.setMat4("u_view_projection", projection * view);

      //Light space transforms
      glm::vec3 forward = glm::normalize(glm::vec3(
        sinf(glm::radians(lightT.rotation_.y)) * cosf(glm::radians(lightT.rotation_.x)),
        sinf(glm::radians(lightT.rotation_.x)),
        -cosf(glm::radians(lightT.rotation_.y)) * cosf(glm::radians(lightT.rotation_.x))
      ));

      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
      glm::vec3 right = glm::normalize(glm::cross(forward, up));
      up = glm::cross(right, forward);
      glm::mat4 lightView = glm::lookAt(lightT.translation_, lightT.translation_ + forward, up);
      glm::mat4 lightSpaceMatrix = light.lightProjection * lightView;

      //Uniforms
      shader.setMat4("u_lightSpaceMatrix", lightSpaceMatrix);


      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();

        shader.setMat4("u_model", transform.model);
        for (const auto& mesh : render.object->model->meshes) {
          glBindVertexArray(mesh.VAO);
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
      }


      //glDisable(GL_DEPTH_TEST);
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // Reset to default framebuffer
    }
    glCullFace(GL_BACK);
  }
}