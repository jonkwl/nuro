#pragma once

#include <cstdint>
#include <string>

#include "../core/resource/resource_task.h"

enum class TextureType
{
	EMPTY,

	ALBEDO,
	ROUGHNESS,
	METALLIC,
	NORMAL,
	OCCLUSION,
	EMISSIVE,
	HEIGHT,

	IMAGE_RGB,
	IMAGE_RGBA,
};

class Texture : ResourceTask
{
public:
	Texture();

	void load() override;
	void upload() override;

	// Sets the texture type and source used when loading and uploading
	void setSource(TextureType type, const std::string& path);

	// Creates the texture synchronously, blocking until complete  
	void createSync();

	// Queues texture creation for asynchronous creation, not blocking
	void createAsync();

	// Returns the textures backend id
	uint32_t getId() const;

private:
	// Texture type
	TextureType type;

	// Texture source path
	std::string path;

	// Texture width
	uint32_t width;

	// Texture height
	uint32_t height;

	// Texture channels
	uint32_t channels;

	// Dynamic temporary texture data
	unsigned char* data;

	// Backend texture id
	uint32_t id;
};