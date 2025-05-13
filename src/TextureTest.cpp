#include <GL/glew.h>
#include "MEW/TextureTest.hpp"
#include <iostream>

namespace MEW {
	TextureData TextureFromFile(const char* path, const std::string& directory) {
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		std::shared_ptr<unsigned char> shared_data{ data, &stbi_image_free };
		TextureData texture_data = { static_cast<size_t>(width), static_cast<size_t>(height), static_cast<size_t>(nrComponents), shared_data };
		return texture_data;
	}

	Texture::Texture(const TextureData& texture_data) {
		terminate_ = true;
		if (texture_data.data_.get()) {
			glGenTextures(1, &id_);

			GLenum format;
			if (texture_data.nrComponents_ == 1) format = GL_RED;
			else if (texture_data.nrComponents_ == 3) format = GL_RGB;
			else format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id_);
			glTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(texture_data.width_), static_cast<GLsizei>(texture_data.height_), 0, format, GL_UNSIGNED_BYTE, texture_data.data_.get());
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}
		else {
			id_ = 0;
		}
	}

	Texture::~Texture() {
		if (terminate_ && id_ != 0) glDeleteTextures(1, &id_);
	}

	const unsigned int Texture::getID() const {
		return id_;
	}

	Texture::Texture(Texture&& other) noexcept : id_(other.id_), terminate_(other.terminate_) {
		other.id_ = 0;
		other.terminate_ = false;
	}

	Texture& Texture::operator=(const Texture& other) {
		id_ = other.id_;
		terminate_ = other.terminate_;
		return *this;
	}

	Texture::Texture(Texture& other) {
		id_ = other.id_;
		terminate_ = other.terminate_;
	}
}