#ifndef __IDENTITY_H__
#define __IDENTITY_H__ 1

#include <string>

namespace MEW {

  typedef struct IdentityComponent {
    std::string name;
  public:
    IdentityComponent(std::string name) : name(name) {}
  }IdentityComponent;

}



#endif