#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <filesystem>

#include <utils/console.h>
#include <utils/ioutils.h>
#include <context/application_context.h>

namespace fs = std::filesystem;

uint32_t Texture::defaultTextureId = 0;

Texture::Texture() : type(TextureType::EMPTY),
path(),
width(0),
height(0),
channels(0),
data(nullptr),
_id(defaultTextureId)
{
}

void Texture::setSource(TextureType _type, const std::string& _path)
{
	// Validate source path
	if (!fs::exists(_path)) {
		Console::out::warning("Texture", "Texture source at '" + _path + "' could not be found");
	}

	type = _type;
	path = _path;
}

uint32_t Texture::id() const
{
	return _id;
}

void Texture::setDefaultTexture(uint32_t textureId)
{
	defaultTextureId = textureId;
}

std::string Texture::sourcePath()
{
	return path;
}

void Texture::loadData()
{
	// Load image data
	int _width, _height, _channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* _data = stbi_load(path.c_str(), &_width, &_height, &_channels, 0);
	if (!_data)
	{
		Console::out::warning("Texture", "Couldn't load data for texture '" + IOUtils::getFilename(path) + "'");
		return;
	}

	// Sync loaded data
	width = _width;
	height = _height;
	channels = _channels;
	data = _data;
}

void Texture::releaseData()
{
	// Don't release data if there is none
	if (!data) return;

	// Free memory allocated for image data
	stbi_image_free(data);
}

void Texture::dispatchGPU()
{
	// Don't dispatch texture if there is no data
	if (!data) return;

	// Generate texture
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Anisotropic filtering
	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

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
		internalFormat = GL_RGB;
		format = GL_RGB;
		break;
	case TextureType::IMAGE_RGBA:
		internalFormat = GL_RGBA;
		format = GL_RGBA;
		break;
	}

	// Buffer image data to texture
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	// Generate textures mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// Undbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}