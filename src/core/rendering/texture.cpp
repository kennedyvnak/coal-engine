#include "enpch.h"
#include "texture.h"
#include <stb_image.h>
#include <GL/glew.h>

namespace engine {
	Texture::Texture(int width, int height)
		: Asset("generated"), _width(width), _height(height), _local_buffer(nullptr), _bpp(0) {
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(): Asset("generated"), _id(0), _local_buffer(nullptr), _width(0), _height(0), _bpp(0) {
	}

	Texture::Texture(const std::string& path)
		: Asset(path), _id(0), _local_buffer(nullptr), _width(0), _height(0), _bpp(0) {
		stbi_set_flip_vertically_on_load(1);
		_local_buffer = stbi_load(path.c_str(), &_width, &_height, &_bpp, 0);

		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _local_buffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (_local_buffer)
			stbi_image_free(_local_buffer);
	}

	Texture::~Texture() {
		glDeleteTextures(1, &_id);
	}

	Ref<Texture> Texture::GenFlatTexture(int width, int height, unsigned int color_bytes) {
		Ref<Texture> tex = create_ref<Texture>(width, height);
		glBindTexture(GL_TEXTURE_2D, tex->_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &color_bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
		return tex;
	}

	void Texture::bind(unsigned int slot) const {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void Texture::unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
