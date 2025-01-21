#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"
#include <vector>
#include "Transform.hpp"
#include <optional>
#include "Render.hpp"

namespace MEW {

  class LightComponent {
  public:
    LightComponent();
    glm::vec3 color;
    glm::vec3 direction;
    
  protected:

  private:
  };


  class LighRenderMultipleSystem {
  public:
    void operator()(const std::vector<std::optional<MEW::TransformComponent>>& vecTrans,
      const std::vector<std::optional<MEW::RenderComponent>>& vecRender,
      const std::vector<std::optional<MEW::LightComponent>>& vecLight,
      Shader& shader);
  };

}

#endif //__LIGHT__