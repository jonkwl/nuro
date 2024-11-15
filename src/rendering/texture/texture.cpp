#include "texture.h"

Texture::Texture(std::string path, TextureType type)
{
	id = 0;
	this->type = type;

	Log::printProcessInfo("Loading texture " + path + "...");

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, channels;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		Log::printError("Texture", "Couldn't load texture data");
		return;
	}

	GLenum internalFormat = GL_SRGB;
	GLenum format = GL_RGB;

	switch (type) {
	case ALBEDO_MAP:
		internalFormat = GL_SRGB;
		format = GL_RGB;
		break;
	case NORMAL_MAP:
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case ROUGHNESS_MAP:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case METALLIC_MAP:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case AMBIENT_OCCLUSION_MAP:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case EMISSION_MAP:
		internalFormat = GL_RGB;
		format = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}

void Texture::bind(unsigned int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}