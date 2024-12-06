#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "../core/utils/log.h"

Texture::Texture() : backendId(0)
{
}

void Texture::bind(unsigned int unit)
{
	// Bind texture to backend
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, backendId);
}

void Texture::destroy()
{
	// Delete texture in backend
	glDeleteTextures(1, &backendId);
	backendId = 0;
}

unsigned int Texture::getBackendId()
{
	return backendId;
}

Texture Texture::empty()
{
	return Texture();
}

Texture Texture::load(std::string path, TextureType type)
{
	// Buffer for new texture id
	unsigned int newId;

	// Start creating texture
	Log::printProcessInfo("Loading texture " + path + "...");

	// Generate texture
	glGenTextures(1, &newId);
	glBindTexture(GL_TEXTURE_2D, newId);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image data
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		Log::printError("Texture", "Couldn't load texture data");
		return Texture(); // Return empty texture
	}

	// Get texture backend format from texture type
	GLenum internalFormat = GL_SRGB;
	GLenum format = GL_RGB;
	switch (type)
	{
	case TextureType::ALBEDO:
		internalFormat = GL_SRGB;
		format = GL_RGB;
		break;
	case TextureType::NORMAL:
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case TextureType::ROUGHNESS:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::METALLIC:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::AMBIENT_OCCLUSION:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::EMISSIVE:
		internalFormat = GL_RGB;
		format = GL_RGB;
	}

	// Buffer image data to texture
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	// Generate textures mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// Undbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Free memory allocated for image data
	stbi_image_free(data);

	// Return Texture instance with backendId of newly created texture
	return Texture(newId);
}

Texture Texture::fromBackendId(unsigned int backendId)
{
	return Texture(backendId);
}

Texture::Texture(unsigned int backendId) : backendId(backendId)
{
}  