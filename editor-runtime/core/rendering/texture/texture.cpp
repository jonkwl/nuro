#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "../core/utils/console.h"

Texture::Texture() : backendId(0)
{
}

void Texture::bind(uint32_t unit) const
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

uint32_t Texture::getBackendId() const
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
	uint32_t newId;

	// Start creating texture
	Console::out::processInfo("Loading texture " + path + "...");

	// Generate texture
	glGenTextures(1, &newId);
	glBindTexture(GL_TEXTURE_2D, newId);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Anisotropic filtering
	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

	// Load image data
	int32_t width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		Console::out::error("Texture", "Couldn't load texture data");
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
	case TextureType::ROUGHNESS:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::METALLIC:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::NORMAL:
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case TextureType::OCCLUSION:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::EMISSIVE:
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case TextureType::HEIGHT:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case TextureType::IMAGE_RGB:
		internalFormat = GL_SRGB;
		format = GL_RGB;
		break;
	case TextureType::IMAGE_RGBA:
		internalFormat = GL_SRGB_ALPHA;
		format = GL_RGBA;
		break;
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

Texture Texture::fromBackendId(uint32_t backendId)
{
	return Texture(backendId);
}

Texture::Texture(uint32_t backendId) : backendId(backendId)
{
}  