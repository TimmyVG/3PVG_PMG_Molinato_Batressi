#include "mew/Inspector.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace MEW {


  Inspector::Inspector(MEW::Window w) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    //io.DisplaySize = { 640,460 };
    ImGui_ImplGlfw_InitForOpenGL(w.window_, true);
    const char* version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);


    ImGui_ImplOpenGL3_Init(version);
    ImGui::StyleColorsDark();
  }

  void Inspector::WindowEntities(){

    bool state = true;
    if(ImGui::Begin("Entities", &state)) {

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
  inline void Inspector::OpenWindow()
  {



  }
}