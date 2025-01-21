#include <GL/glew.h>
#include "mew/Light.hpp"
#include <gl/GL.h>

namespace MEW {


  LightComponent::LightComponent(){
    color = glm::vec3(0.0f);
    direction = glm::vec3(0.0f);
  }
  // La primera pasada se renderiza sobreescribiendo(GL_ONE, GL_ZERO),
  // Pero a partir de la segunda se renderiza aditivamente(GL_ONE, GL_ONE)
  void LighRenderMultipleSystem::operator()(
    const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
    const std::vector<std::optional<MEW::RenderComponent>>& vecRender, 
    const std::vector<std::optional<MEW::LightComponent>>& vecLight,
    Shader& shader){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    auto itT = vecTrans.begin();
    
    auto itL = vecLight.begin();

    bool first = false;
    auto itLight = vecLight.begin();
    glBlendFunc(GL_ONE, GL_ZERO);
    for (; itLight != vecLight.end() ;  itLight++) {
      if (!itL->has_value()) continue;

      auto itTransform = vecTrans.begin();
      auto itRender = vecRender.begin();
      for (; itTransform != vecTrans.end() && itRender != vecRender.end(); itTransform++, itRender++) {
        if (!itTransform->has_value()) continue;
        if (!itRender->has_value()) continue;

        //Renderizo
      }
      glBlendFunc(GL_ONE, GL_ONE);
    }
    glDisable(GL_BLEND);
  }
}
