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
    std::optional<CameraComponent>& camComp,
    std::optional<TransformComponent>& camCompT) {

    if (!camComp.has_value() || !camCompT.has_value()) return;
    auto caCamera = camComp.value();
    auto trCamera = camCompT.value();

    //General settings
    shader.UseProgram();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ZERO);

    //General Uniform
    shader.setMat4("u_view_projection", caCamera.projectionMatrix * caCamera.viewMatrix);
    shader.setFloat3("u_camera_pos", &trCamera.translation_.x);
    shader.setFloat3("u_camera_dir", &trCamera.fwd.x);


    // Iterate through each light
    auto itTransformL = vecTrans.begin();
    auto itLightL = vecLight.begin();

    for (; itTransformL != vecTrans.end()  && itLightL != vecLight.end(); itTransformL++, itLightL++) {
      if (!itTransformL->has_value()  || !itLightL->has_value()) continue;
      auto trLight = itTransformL->value();
      auto liLight = itLightL->value();

      //Light Uniforms
      shader.setInt("u_type", liLight.type);

      shader.setFloat3("u_diffuse_color", &liLight.diffuse.x);
      shader.setFloat("u_diffuse_strength", liLight.fDiffuse);

      shader.setFloat("u_constant", liLight.constant);
      shader.setFloat("u_linear", liLight.linear);
      shader.setFloat("u_quadratic", liLight.quadratic);


      shader.setFloat3("u_spec_color", &liLight.specular.x);
      shader.setFloat("u_spec_strength", liLight.fSpecular);

      shader.setFloat("u_cutoff", glm::cos(glm::radians(liLight.cutOff)));
      shader.setFloat("u_outercutoff", glm::cos(glm::radians(liLight.outerCutOff)));

      shader.setFloat3("u_light_dir", &trLight.fwd.x);
      shader.setFloat3("u_lightPos", &trLight.translation_.x);
      shader.setMat4("u_lightSpaceMatrix", trLight.mat_);
      // Now we draw each object
      auto itTransformO = vecTrans.begin();
      auto itRenderO = vecRender.begin();
      for (; itTransformO != vecTrans.end() && itRenderO != vecRender.end(); itTransformO++, itRenderO++) {
        if (!itTransformO->has_value() || !itRenderO->has_value()) continue;
        auto trObject = itTransformO->value();
        auto reObject = itRenderO->value();
        
        // Draw
        shader.setMat4("u_model", trObject.model);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(trObject.model)));
        shader.setMat4("u_normalMatrix", normalMatrix);
        
        //Material prop
        shader.setFloat("u_shininess", liLight.shininess);
        for (const auto& mesh : reObject.object->model->meshes)
        {
          // Draw mesh
          unsigned int diffuseNr = 1;
          for (unsigned int j = 0; j < mesh.textures_.size(); j++) {
            glActiveTexture(GL_TEXTURE0 + j);
            std::string number;
            std::string name = mesh.textures_[j].type;
            if (name == "texture_diffuse") {
              number = std::to_string(diffuseNr++);
            }
            shader.setInt((name + number).c_str(), j);
            glBindTexture(GL_TEXTURE_2D, mesh.textures_[j].id);
          }
          glActiveTexture(GL_TEXTURE0);
          glBindVertexArray(mesh.VAO);   
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
      }
      glBlendFunc(GL_ONE, GL_ONE);
    }
    glDisable(GL_BLEND);
  }
  
    void LightSystem::operator()(
      const std::vector<std::optional<MEW::TransformComponent>>&vecTrans,
      const std::vector<std::optional<MEW::RenderComponent>>&vecRender,
      std::vector<std::optional<MEW::LightComponent>>&vecLight,
      Shader & shader,
      std::optional<CameraComponent>&camComp) {


    }
}