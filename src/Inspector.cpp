#include "mew/Inspector.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
namespace MEW {



  Inspector::Inspector() {
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
  }

  void Inspector::WindowEntities(){
    ImGui::NewFrame();
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
   // glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  template<typename T>
  inline void Inspector::OpenWindow()
  {



  }
}