#include <GL/glew.h>
#include "mew/Render.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>


#define CAMERA_WIDTH (1280)
#define CAMERA_HEIGHT (720)
#define SHADOW_WIDTH (1024)
#define SHADOW_HEIGHT (1024)
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

	void RenderSystemUnlit::operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
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
    glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);
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

    for (; itTransformL != vecTrans.end() && itLightL != vecLight.end(); itTransformL++, itLightL++) {
      if (!itTransformL->has_value() || !itLightL->has_value()) continue;
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

      shader.setInt("u_blin", liLight.bling);

      shader.setFloat3("u_light_dir", &trLight.fwd.x);
      shader.setFloat3("u_lightPos", &trLight.translation_.x);
      shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
      shader.setFloat("u_near", liLight.near_plane);
      shader.setFloat("u_far", liLight.far_plane);
      shader.setInt("u_shadowMap", 10);
      shader.setInt("u_cubeMap", 11);
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
        for (const auto& mesh : reObject.model->value().meshes_)
        {
          // Draw mesh
          unsigned int diffuseNr = 1;
          unsigned int textureUnit = 1;
          if (mesh.diffuse_tex_.has_value()) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            shader.setInt("texture_diffuse0", textureUnit);
            glBindTexture(GL_TEXTURE_2D, mesh.diffuse_tex_.value().getID());
          }

          glActiveTexture(GL_TEXTURE0);

          if (KTypeLight::Point == liLight.type) {
            glActiveTexture(GL_TEXTURE0 + 11);
            glBindTexture(GL_TEXTURE_CUBE_MAP, liLight.depthMap);

          }
          else {
            glActiveTexture(GL_TEXTURE0 + 10);
            glBindTexture(GL_TEXTURE_2D, liLight.depthMap);

          }
          glBindVertexArray(mesh.GetVAO());
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }
      }
      glBlendFunc(GL_ONE, GL_ONE);
    }
    glDisable(GL_BLEND);
  }

  void LightSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader, Shader& shaderCube,
    std::optional<CameraComponent>& camComp) {
    if (!camComp.has_value()) return;


    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto& liLight = itLight->value();
      auto& trLight = itLightT->value();
      //liLight.lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, liLight.near_plane, liLight.far_plane);


      if (KTypeLight::Directional == liLight.type) {
        shader.UseProgram();
        liLight.lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, liLight.near_plane, liLight.far_plane);

        //liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);

      }
      if (KTypeLight::Spot == liLight.type) {
        shader.UseProgram();
        liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);

      }
      if (KTypeLight::Point == liLight.type) {
        shaderCube.UseProgram();
        liLight.lightProjection = glm::perspective(glm::radians(90.0f), (float)liLight.shadow_width / (float)liLight.shadow_height, liLight.near_plane, liLight.far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(liLight.lightProjection *
          glm::lookAt(trLight.translation_, trLight.translation_ + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        for (unsigned int i = 0; i < 6; ++i) {

          std::string uniformName = "shadowMatrices[" + std::to_string(i) + "]";
          shader.setMat4(uniformName.c_str(), shadowTransforms[i]);
        }
      }

      liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
      liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;

      shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
      shader.setFloat("u_far", liLight.far_plane);
      shader.setInt("u_type", liLight.type);
      shader.setFloat3("u_lightPos", &trLight.translation_.x);
      glViewport(0, 0, liLight.shadow_width, liLight.shadow_height);
      glBindFramebuffer(GL_FRAMEBUFFER, liLight.depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();
        shader.setMat4("u_model", transform.model);
        for (const auto& mesh : render.model->value().meshes_)
        {
          // Draw mesh
          unsigned int diffuseNr = 1;

          glBindVertexArray(mesh.GetVAO());
          glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);
        }

      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      std::cout << "OpenGL Error: " << err << std::endl;
    }
    glCullFace(GL_BACK);
  }
}