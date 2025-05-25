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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, camComp->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.UseProgram();

    glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

    //General Uniform
    shader.setMat4("u_view_projection", caCamera.projectionMatrix * caCamera.viewMatrix);

    auto itTransformO = vecTrans.begin();
    auto itRenderO = vecRender.begin();
    for (; itTransformO != vecTrans.end() && itRenderO != vecRender.end(); itTransformO++, itRenderO++) {
      if (!itTransformO->has_value() || !itRenderO->has_value()) continue;
      auto trObject = itTransformO->value();
      auto reObject = itRenderO->value();

      shader.setMat4("u_model", trObject.model);
      glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(trObject.model)));
      shader.setMat4("u_normalMatrix", normalMatrix);

      for (const auto& mesh : reObject.model->value().meshes_)
      {
        unsigned int diffuseNr = 0;
        unsigned int textureUnit = 0;
        if (mesh.diffuse_tex_.has_value()) {
          glActiveTexture(GL_TEXTURE0 + textureUnit);
          shader.setInt("texture_diffuse0", textureUnit);
          glBindTexture(GL_TEXTURE_2D, mesh.diffuse_tex_.value().getID());
        }

        glBindVertexArray(mesh.GetVAO());
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBlendFunc(GL_ONE, GL_ONE);
        glActiveTexture(GL_TEXTURE0);
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
  }

  void RenderSystemLitSSAO::operator()(
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, camComp->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.UseProgram();

    glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

    //General Uniform
    shader.setMat4("u_view", caCamera.viewMatrix);
    shader.setMat4("u_view_projection", caCamera.projectionMatrix * caCamera.viewMatrix);

    auto itTransformO = vecTrans.begin();
    auto itRenderO = vecRender.begin();
    for (; itTransformO != vecTrans.end() && itRenderO != vecRender.end(); itTransformO++, itRenderO++) {
      if (!itTransformO->has_value() || !itRenderO->has_value()) continue;
      auto trObject = itTransformO->value();
      auto reObject = itRenderO->value();

      shader.setMat4("u_model", trObject.model);
      glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(trObject.model)));
      shader.setMat4("u_normalMatrix", normalMatrix);

      for (const auto& mesh : reObject.model->value().meshes_)
      {
        unsigned int diffuseNr = 0;
        unsigned int textureUnit = 0;
        if (mesh.diffuse_tex_.has_value()) {
          glActiveTexture(GL_TEXTURE0 + textureUnit);
          shader.setInt("texture_diffuse0", textureUnit);
          glBindTexture(GL_TEXTURE_2D, mesh.diffuse_tex_.value().getID());
        }

        glBindVertexArray(mesh.GetVAO());
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices_.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBlendFunc(GL_ONE, GL_ONE);
        glActiveTexture(GL_TEXTURE0);
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
  }


  void ForwardRenderSystemLit::operator()(
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

  void ForwardLightSystem::operator()(
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
        glm::vec3 center = trLight.translation_;
        glm::vec3 lightPos = center - trLight.fwd * 1.0f;
        liLight.lightView = glm::lookAt(lightPos, center, glm::vec3(0, 1, 0));
        float size = 40.0f;
        liLight.lightProjection = glm::ortho(-size, size, -size, size, liLight.near_plane, liLight.far_plane);

        //liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);
      }
      if (KTypeLight::Spot == liLight.type) {
        shader.UseProgram();
        liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);
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
          shaderCube.setMat4(uniformName.c_str(), shadowTransforms[i]);
        }
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shaderCube.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shaderCube.setFloat("u_far", liLight.far_plane);
        shaderCube.setInt("u_type", liLight.type);
        shaderCube.setFloat3("u_lightPos", &trLight.translation_.x);
      }



      glViewport(0, 0, liLight.shadow_width, liLight.shadow_height);
      glBindFramebuffer(GL_FRAMEBUFFER, liLight.depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();
        if (KTypeLight::Point == liLight.type) {
          shaderCube.setMat4("u_model", transform.model);
        }
        else {

          shader.setMat4("u_model", transform.model);
        }
        for (const auto& mesh : render.model->value().meshes_)
        {
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
  PhysicsRenderSystem::PhysicsRenderSystem() {
    isInitialized = false;
    m_debugMode = DBG_DrawWireframe;
    InitializeDebugDrawing();
  }
  PhysicsRenderSystem::~PhysicsRenderSystem()
  {
    if (isInitialized)
    {
      glDeleteVertexArrays(1, &m_debugVAO);
      glDeleteBuffers(1, &m_debugVBO);
    }
  }
  void PhysicsRenderSystem::InitializeDebugDrawing()
  {
    if (isInitialized) return;

    glGenVertexArrays(1, &m_debugVAO);
    glBindVertexArray(m_debugVAO);

    glGenBuffers(1, &m_debugVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    isInitialized = true;
  }

  void PhysicsRenderSystem::DrawLine(const glm::vec3& from,
    const glm::vec3& to,
    Shader& shader,
    CameraComponent& camComp,
    const glm::vec3& color,
    float lineWidth
  ) {


    if (m_debugVAO == 0 || m_debugVBO == 0) {
      printf("ERROR: Debug VAO/VBO not initialized!");
      return;
    }
    glLineWidth(lineWidth);
    float vertices[] = {
        from.x, from.y, from.z,
        to.x, to.y, to.z
    };

    shader.UseProgram();
    glm::mat4 view = camComp.viewMatrix;
    glm::mat4 projection = camComp.projectionMatrix;
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setFloat3("color", &color.x);

    // Draw the line
    glBindVertexArray(m_debugVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glLineWidth(1.0f);
  }
  void PhysicsRenderSystem::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
    if (!m_shader || !m_cam) return;

    DrawLine(
      glm::vec3(from.x(), from.y(), from.z()),
      glm::vec3(to.x(), to.y(), to.z()),
      *m_shader,
      *m_cam,
      glm::vec3(color.x(), color.y(), color.z())
    );
  }

  void PhysicsRenderSystem::reportErrorWarning(const char* warningString) {
    std::cerr << "[Bullet Debug Warning] " << warningString << std::endl;
  }

  void PhysicsRenderSystem::setDebugMode(int debugMode) {
    m_debugMode = debugMode;
  }

  int PhysicsRenderSystem::getDebugMode() const {
    return m_debugMode;
  }

  void PhysicsRenderSystem::SetShaderAndCamera(Shader* shader, CameraComponent* cam) {
    m_shader = shader;
    m_cam = cam;
  }


  void LightSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader, Shader& shaderCube,
    std::optional<CameraComponent>& camComp, std::optional<TransformComponent>& camCompT) {
    if (!camComp.has_value()) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.UseProgram();
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ZERO);
    glEnable(GL_DEPTH_TEST);

    shader.setInt("gPosition", 0);
    shader.setInt("gNormal", 1);
    shader.setInt("gColorSpec", 2);
    shader.setInt("u_cubeMap", 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, camComp->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, camComp->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, camComp->gColorSpec);

    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto& liLight = itLight->value();
      auto& trLight = itLightT->value();


      if (KTypeLight::Point == liLight.type) {
       // shaderCube.UseProgram();
      }

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
      shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
      shader.setFloat("u_far", liLight.far_plane);
      shader.setInt("u_type", liLight.type);
      shader.setFloat3("u_lightPos", &trLight.translation_.x);
      shader.setFloat3("u_camera_pos", &camCompT->translation_.x);
      shader.setFloat3("u_camera_dir", &camCompT->fwd.x);
      shader.setFloat("u_shininess", liLight.shininess);

      if (KTypeLight::Point == liLight.type) {
        glActiveTexture(GL_TEXTURE0 + 11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, liLight.depthMap);

      }
      else {
        glActiveTexture(GL_TEXTURE0 + 10);
        glBindTexture(GL_TEXTURE_2D, liLight.depthMap);

      }
      if (camComp->quadVAO == 0)
      {
        float quadVertices[] = {
          // positions        // texture Coords
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &camComp->quadVAO);
        glGenBuffers(1, &camComp->quadVBO);
        glBindVertexArray(camComp->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, camComp->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
      }

      glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

      glBindVertexArray(camComp->quadVAO);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
      glBlendFunc(GL_ONE, GL_ONE);
    }

    glDisable(GL_BLEND);


    glBindFramebuffer(GL_READ_FRAMEBUFFER, camComp->gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

    glBlitFramebuffer(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, 0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  }

  void LightSystemSSAO::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader, Shader& shaderCube,Shader& shaderHDR,
    std::optional<CameraComponent>& camComp, std::optional<TransformComponent>& camCompT) {
    if (!camComp.has_value()) return;
    glBindFramebuffer(GL_FRAMEBUFFER, camComp->hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.UseProgram();
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_ONE, GL_ZERO);
    glEnable(GL_DEPTH_TEST);

    shader.setInt("gPosition", 0);
    shader.setInt("gNormal", 1);
    shader.setInt("gColorSpec", 2);
    shader.setInt("ssao", 3);
    shader.setInt("u_ssao", camComp.value().ssao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, camComp->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, camComp->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, camComp->gColorSpec);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, camComp->ssaoColorBufferBlur);
    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto& liLight = itLight->value();
      auto& trLight = itLightT->value();


      if (KTypeLight::Point == liLight.type) {
        // shaderCube.UseProgram();
      }

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
      //glm::vec3 lightDir_view = glm::mat3(camComp.value().viewMatrix) * trLight.fwd;
      shader.setFloat3("u_light_dir", &trLight.fwd.x);
      //glm::vec3 lightPos_view = glm::vec3(camComp.value().viewMatrix * glm::vec4(trLight.translation_, 1.0f));
      shader.setFloat3("u_lightPos", &trLight.translation_.x);
      shader.setMat4("u_viewInv", glm::inverse(camComp.value().viewMatrix));
      shader.setMat4("u_view", camComp.value().viewMatrix);
      shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
      shader.setFloat("u_near", liLight.near_plane);
      shader.setFloat("u_far", liLight.far_plane);
      shader.setInt("u_shadowMap", 10);
      shader.setInt("u_cubeMap", 11);

      shader.setInt("u_type", liLight.type);
      shader.setFloat3("u_camera_pos", &camCompT->translation_.x);
      shader.setFloat3("u_camera_dir", &camCompT->fwd.x);
      shader.setFloat("u_shininess", liLight.shininess);

      if (KTypeLight::Point == liLight.type) {
        glActiveTexture(GL_TEXTURE0 + 11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, liLight.depthMap);

      }
      else {
        glActiveTexture(GL_TEXTURE0 + 10);
        glBindTexture(GL_TEXTURE_2D, liLight.depthMap);

      }
      if (camComp->quadVAO == 0)
      {
        float quadVertices[] = {
          // positions        // texture Coords
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &camComp->quadVAO);
        glGenBuffers(1, &camComp->quadVBO);
        glBindVertexArray(camComp->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, camComp->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
      }

      glViewport(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT);

      glBindVertexArray(camComp->quadVAO);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
      glBlendFunc(GL_ONE, GL_ONE);
    }

    glDisable(GL_BLEND);

    //hdr
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderHDR.UseProgram();
    shaderHDR.setInt("hdrBuffer", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, camComp->hdrColorBuffer);
    shaderHDR.setInt("hdr",camComp->hdr);
    shaderHDR.setFloat("exposure", camComp->exposure);

    if (camComp->quadVAO == 0)
    {
      float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &camComp->quadVAO);
      glGenBuffers(1, &camComp->quadVBO);
      glBindVertexArray(camComp->quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, camComp->quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(camComp->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, camComp->gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

    glBlitFramebuffer(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, 0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
  }

  void RenderSSAOTexture::operator()(Shader& shader,
                                     Shader& shaderBlur,
                                     std::optional<CameraComponent>& cameraCamera,
                                     std::optional<TransformComponent>& cameraTransform) {

    if (!cameraCamera.has_value()) return;
    if (!cameraTransform.has_value()) return;


    glBindFramebuffer(GL_FRAMEBUFFER, cameraCamera->ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    shader.UseProgram();
    shader.setInt("gPosition", 0);
    shader.setInt("gNormal", 1);
    shader.setInt("texNoise", 2);
    for (unsigned int i = 0; i < 64; ++i) {
      std::string uniformName = "u_samples[" + std::to_string(i) + "]";
      shader.setFloat3(uniformName.c_str(), &cameraCamera.value().ssaoKernel[i].x);
    }
    shader.setMat4("u_projection", cameraCamera.value().projectionMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cameraCamera.value().gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cameraCamera.value().gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cameraCamera.value().noiseTexture);
    

    shader.setInt("kernelSize", cameraCamera.value().kernelSize);
    shader.setFloat("radius", cameraCamera.value().radius);
    shader.setFloat("bias", cameraCamera.value().bias);
    //RenderQuad

    if (cameraCamera.value().quadVAO == 0)
    {
      float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &cameraCamera.value().quadVAO);
      glGenBuffers(1, &cameraCamera.value().quadVBO);
      glBindVertexArray(cameraCamera.value().quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, cameraCamera.value().quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(cameraCamera.value().quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ///////////////////////BLUR///////////////////////////////////////////
    if (cameraCamera.value().blur) {
      glBindFramebuffer(GL_FRAMEBUFFER, cameraCamera.value().ssaoBlurFBO);
      glClear(GL_COLOR_BUFFER_BIT);
      shaderBlur.UseProgram();
      shaderBlur.setInt("ssaoInput", 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, cameraCamera.value().ssaoColorBuffer);

      //RenderQUAD
      if (cameraCamera.value().quadVAO == 0)
      {
        float quadVertices[] = {
          // positions        // texture Coords
          -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
          -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
           1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
           1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &cameraCamera.value().quadVAO);
        glGenBuffers(1, &cameraCamera.value().quadVBO);
        glBindVertexArray(cameraCamera.value().quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cameraCamera.value().quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
      }
      glBindVertexArray(cameraCamera.value().quadVAO);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }
  }
  

  void DepthMaps::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader, Shader& shaderCube,
    std::optional<CameraComponent>& camComp,
    std::optional<TransformComponent>& camT) {
    if (!camComp.has_value()) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto& liLight = itLight->value();
      auto& trLight = itLightT->value();

      if (KTypeLight::Directional == liLight.type) {
        shader.UseProgram();
        glm::vec3 center = trLight.translation_;
        glm::vec3 lightPos = center - trLight.fwd * 1.0f;
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        float size = 40.0f;
        liLight.lightProjection = glm::ortho(-size, size, -size, size, liLight.near_plane, liLight.far_plane);
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);
  

      }
      if (KTypeLight::Spot == liLight.type) {
        shader.UseProgram();
        liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);


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
          shaderCube.setMat4(uniformName.c_str(), shadowTransforms[i]);
        }
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shaderCube.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shaderCube.setFloat("u_far", liLight.far_plane);
        shaderCube.setInt("u_type", liLight.type);
        shaderCube.setFloat3("u_lightPos", &trLight.translation_.x);


      }

      glViewport(0, 0, liLight.shadow_width, liLight.shadow_height);
      glBindFramebuffer(GL_FRAMEBUFFER, liLight.depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      //Recorrer meshes
      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();
        if (liLight.type == KTypeLight::Point) {
          shaderCube.setMat4("u_model", transform.model);

        }
        else {

        shader.setMat4("u_model", transform.model);
        }
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

  void DepthMapsSSAO::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
    std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader, Shader& shaderCube,
    std::optional<CameraComponent>& camComp,
    std::optional<TransformComponent>& camT) {
    if (!camComp.has_value()) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto itLight = vecLight.begin();
    auto itLightT = vecTrans.begin();
    for (; itLight != vecLight.end() && itLightT != vecTrans.end(); itLight++, itLightT++) {
      if (!itLight->has_value()) continue;
      if (!itLightT->has_value()) continue;
      auto& liLight = itLight->value();
      auto& trLight = itLightT->value();

      if (KTypeLight::Directional == liLight.type) {
        shader.UseProgram();
        glm::vec3 center = trLight.translation_;
        glm::vec3 lightPos = center - trLight.fwd * 1.0f;
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        float size = 40.0f;
        liLight.lightProjection = glm::ortho(-size, size, -size, size, liLight.near_plane, liLight.far_plane);
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);


      }
      if (KTypeLight::Spot == liLight.type) {
        shader.UseProgram();
        liLight.lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, liLight.near_plane, liLight.far_plane);
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shader.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shader.setFloat("u_far", liLight.far_plane);
        shader.setInt("u_type", liLight.type);
        shader.setFloat3("u_lightPos", &trLight.translation_.x);


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
          shaderCube.setMat4(uniformName.c_str(), shadowTransforms[i]);
        }
        liLight.lightView = glm::lookAt(trLight.translation_, trLight.translation_ + trLight.fwd, glm::vec3(0.0, 1.0, 0.0));
        liLight.lightSpaceMatrix = liLight.lightProjection * liLight.lightView;
        shaderCube.setMat4("u_lightSpaceMatrix", liLight.lightSpaceMatrix);
        shaderCube.setFloat("u_far", liLight.far_plane);
        shaderCube.setInt("u_type", liLight.type);
        shaderCube.setFloat3("u_lightPos", &trLight.translation_.x);


      }

      glViewport(0, 0, liLight.shadow_width, liLight.shadow_height);
      glBindFramebuffer(GL_FRAMEBUFFER, liLight.depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);

      //Recorrer meshes
      auto itRender = vecRender.begin();
      auto itTransform = vecTrans.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itRender->has_value() || !itTransform->has_value()) continue;
        auto& render = itRender->value();
        auto& transform = itTransform->value();
        if (liLight.type == KTypeLight::Point) {
          shaderCube.setMat4("u_model", transform.model);

        }
        else {

          shader.setMat4("u_model", transform.model);
        }
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

  void WaterRenderSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTransform,
    const std::vector<std::optional<MEW::WaterComponent>>& vecWater,
    Shader& shader,
    std::optional<CameraComponent>& camComp,float currentTime){
    for (size_t i = 0; i < vecWater.size(); ++i) {
      if (!vecTransform[i] || !vecWater[i]) continue;

      auto& water = *vecWater[i];
      auto& transform = *vecTransform[i];

      shader.UseProgram();

      shader.setMat4("model", transform.model);
      shader.setMat4("view", camComp->viewMatrix);
      shader.setMat4("projection", camComp->projectionMatrix);

      auto& mesh = water.mesh->value();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mesh.diffuse_tex_.value().getID());

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, mesh.normal_tex_.value().getID());

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, mesh.displacement_tex.value().getID());

      shader.setInt("waterDiffuse", 0);
      shader.setInt("waterNormal", 1);
      shader.setInt("displacementMap", 2);
      shader.setFloat("time", currentTime);

      glBindVertexArray(water.mesh->value().GetVAO());
      glDrawElements(GL_TRIANGLES, water.mesh->value().indices_.size(), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }
  }
}