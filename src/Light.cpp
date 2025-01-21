#include <GL/glew.h>
#include "mew/Light.hpp"
#include <gl/GL.h>

namespace MEW {


  LightComponent::LightComponent(){
    color = glm::vec3(0.0f);
    direction = glm::vec3(0.0f);
  }

}
