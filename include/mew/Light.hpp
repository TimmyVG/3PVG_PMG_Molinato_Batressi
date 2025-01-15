#ifndef __LIGHT_H__
#define __LIGHT_H__ 1

#include "glm/vec3.hpp"

namespace MEW {

  class Light {
  public:
    Light();
    glm::vec3 color;
    glm::vec3 direction;
    
  protected:

  private:
  };

}

#endif //__LIGHT__