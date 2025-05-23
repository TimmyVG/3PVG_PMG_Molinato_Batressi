#include "mew/Inspector.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <mew/Identity.hpp>
#include <mew/Transform.hpp>
#include <mew/Light.hpp>

#include "glm/gtc/type_ptr.hpp"
namespace MEW {



  Inspector::Inspector(MEW::Window &w) {
    ecs = nullptr;
    EntityInspector = -1;
    InspectorEntitiesVisible = true;
    InspectorEntityVisible = true;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //io.DisplaySize = { 640,460 };
    ImGui_ImplGlfw_InitForOpenGL(w.window_, true);



    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
  }

  void Inspector::update(float deltaTime, Input& inputManager)
  {
    if (inputManager.isKeyPressed(ActionsInspector::CLICK_OUT)) {
      if (!ImGui::IsAnyItemActive() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
        ImGui::SetKeyboardFocusHere(-1);
      }
    }
  }

  void Inspector::LinkECS(MEW::ECSManager &ecs)
  {
    this->ecs = &ecs;
  }

  void Inspector::NewFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
  }



  void Inspector::WindowEntities(MEW::Camera caCamera){

    bool state = true;
    if(ImGui::Begin("Entities", &InspectorEntitiesVisible)) {

      const auto &vecIdentity = ecs->get_vectorComponent<MEW::IdentityComponent>();
      auto itIdentity = vecIdentity.begin();
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

      for (size_t sEntity = 0; itIdentity != vecIdentity.end(); itIdentity++, sEntity++) {
        if (!itIdentity->has_value()) continue;
        if (ImGui::Button(itIdentity->value().name.c_str(), { ImGui::GetContentRegionAvail().x,0.0f })) {
          EntityInspector = sEntity;
        }
      }
      ImGui::PopStyleVar();
    }
    ImGui::End();

    if (EntityInspector != -1) {
      auto identityC =  ecs->get_component<MEW::IdentityComponent>(EntityInspector);
      if (!identityC.has_value()) return;
      ImGuiIO& io = ImGui::GetIO();
      float width = 300.0f;
      ImVec2 pos = ImVec2(
        (io.DisplaySize.x - width) ,
        (0.0f)
      );
      ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
      ImGui::SetNextWindowSize({ width, ImGui::GetIO().DisplaySize.y });
      if (ImGui::Begin(identityC.value().name.c_str(), &InspectorEntityVisible)) {

        auto transform = &ecs->get_component<MEW::TransformComponent>(EntityInspector);
        if (transform->has_value()) {
          ImGui::Text("Transform");
          if (ImGui::InputFloat3("Position", &transform->value().translation_.x)) {}
          if (ImGui::InputFloat3("Rotation", &transform->value().rotation_.x)) {}
          if (ImGui::InputFloat3("Scale", &transform->value().scale_.x)) {}
        }

        auto light = &ecs->get_component<MEW::LightComponent>(EntityInspector);
        auto lightT = &ecs->get_component<MEW::TransformComponent>(EntityInspector);

        if (light->has_value()) {
          ImGui::Text("Light");
          if (light->value().type == KTypeLight::Directional) {
            ImGui::Text("Directional");
            if (ImGui::ColorPicker3("Color Diffuse", &light->value().diffuse.x)) {}
            if (ImGui::InputFloat("Diffuse Strenght", &light->value().fDiffuse)) {}
            if (ImGui::ColorPicker3("Color Spec", &light->value().specular.x)) {}
            if (ImGui::InputFloat("Spec Strenght", &light->value().fSpecular)) {}
            if (ImGui::InputFloat("Near", &light->value().near_plane)) {}
            if (ImGui::InputFloat("Far", &light->value().far_plane)) {}
            if (ImGui::InputFloat("Shininess", &light->value().shininess)) {}
          }
          else if (light->value().type == KTypeLight::Spot) {
            ImGui::Text("Spot");
            if (ImGui::ColorPicker3("Color Diffuse", &light->value().diffuse.x)) {}
            if (ImGui::InputFloat("Diffuse Strenght", &light->value().fDiffuse)) {}
            if (ImGui::ColorPicker3("Color Spec", &light->value().specular.x)) {}
            if (ImGui::InputFloat("Spec Strenght", &light->value().fSpecular)) {}
            if (ImGui::InputFloat("Near", &light->value().near_plane)) {}
            if (ImGui::InputFloat("Far", &light->value().far_plane)) {}
            if (ImGui::InputFloat("Shininess", &light->value().shininess)) {}
          }
          else if (light->value().type == KTypeLight::Point) {
            ImGui::Text("Point");
            if (ImGui::ColorPicker3("Color Diffuse", &light->value().diffuse.x)) {}
            if (ImGui::InputFloat("Diffuse Strenght", &light->value().fDiffuse)) {}
            if (ImGui::ColorPicker3("Color Spec", &light->value().specular.x)) {}
            if (ImGui::InputFloat("Spec Strenght", &light->value().fSpecular)) {}
            if (ImGui::InputFloat("Near", &light->value().near_plane)) {}
            if (ImGui::InputFloat("Far", &light->value().far_plane)) {}
            if (ImGui::InputFloat("Shininess", &light->value().shininess)) {}
          }
          else if (light->value().type == KTypeLight::Ambient) {
            ImGui::Text("Ambient");
            if (ImGui::ColorPicker3("Color Diffuse", &light->value().diffuse.x)) {}
            if (ImGui::InputFloat("Diffuse Strenght", &light->value().fDiffuse)) {}
          }
          // Luego pasas el arreglo a ImGui::ColorPicker3
          if (caCamera.GetCameraComponent()->ssao == 1) {

          }




          bool blingValue = light->value().bling;  // Copia el valor
          if (ImGui::Checkbox("Blin", &blingValue)) {
            light->value().bling = blingValue;  // Actualiza el valor original
          }
        }  
      }

      auto cameraEntity = &ecs->get_component<MEW::CameraComponent>(EntityInspector);
      if (EntityInspector != -1 && cameraEntity->has_value()) {
        ImGui::InputInt("Blur", &cameraEntity->value().blur);
        ImGui::InputInt("SSAO", &cameraEntity->value().ssao);
        ImGui::InputInt("kernel", &cameraEntity->value().kernelSize);
        ImGui::InputFloat("radius", &cameraEntity->value().radius);
        ImGui::InputFloat("bias", &cameraEntity->value().bias);

      }

      auto trEntity = &ecs->get_component<MEW::TransformComponent>(EntityInspector);
      if (EntityInspector != -1  && trEntity->has_value()) {
        ImGuiIO& io = ImGui::GetIO();

        if (io.KeysDown[GLFW_KEY_T]) operation = ImGuizmo::TRANSLATE; // Mover
        if (io.KeysDown[GLFW_KEY_R]) operation = ImGuizmo::ROTATE;    // Rotar
        if (io.KeysDown[GLFW_KEY_S]) operation = ImGuizmo::SCALE;

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
        float windowWidth = (float)1280;
        float windowHeight = (float)720;

        glm::mat4 cameraView = caCamera.GetCameraComponent()->viewMatrix;
        glm::mat4 projection = caCamera.GetCameraComponent()->projectionMatrix;
        ImGuizmo::SetRect(0,0, windowWidth, windowHeight);

        ImGuizmo::Manipulate(glm::value_ptr(cameraView),glm::value_ptr(projection),
          operation,mCurrentGizmoMode,glm::value_ptr(trEntity->value().model));

        if (ImGuizmo::IsUsing()) {
          float matrixTranslation[3], matrixRotation[3], matrixScale[3];
          ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(trEntity->value().model), matrixTranslation, matrixRotation, matrixScale);
          //ImGui::InputFloat3("Tr", matrixTranslation);
          //ImGui::InputFloat3("Rt", matrixRotation);
          //ImGui::InputFloat3("Sc", matrixScale);
          ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(trEntity->value().model));
          trEntity->value().translation_ = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
          trEntity->value().rotation_ = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
          trEntity->value().scale_ = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
          
        }
      }

      ImGui::End();
    }
  }

  void Inspector::HideAll()
  {
  }

  void Inspector::ShowAll()
  {
  }

  void Inspector::Render()
  {
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  Inspector::~Inspector()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  template<typename T>
   void Inspector::TextComponent()
  {



  }
}