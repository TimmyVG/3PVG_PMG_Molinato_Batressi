
#ifndef __TEXTURE__H__
#define __TEXTURE__H__

#include "Shader.hpp"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <optional>
#include <memory>

#include <stb_image.h>
namespace MEW {
  struct TextureData {
    size_t height_, width_, nrComponents_;
    std::shared_ptr<unsigned char> data_;
    std::string cadena;
  };


  class Texture {
  public:
    Texture(const TextureData&);
    ~Texture();
    Texture(Texture&& other) noexcept;
    const unsigned int getID() const;
    Texture& operator=(const Texture& other);
    Texture(Texture& other);

  private:
    unsigned int id_;
    bool terminate_;
  };

  TextureData TextureFromFile(const char* path, const std::string& directory);
}
#endif

