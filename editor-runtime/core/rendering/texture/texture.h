#pragma once

#include <cstdint>
#include <string>

#include "../core/asset/asset.h"

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

class Texture : public Asset
{
public:
	Texture();

	void load() override;
	void upload() override;
	std::string sourcePath() override;

	// Sets the texture type and source used when loading and uploading
	void setSource(TextureType type, const std::string& path);

	// Returns the textures backend id
	uint32_t getId() const;

	// Sets the given textures backend id to be the default backend id for new textures
	static void setDefaultTexture(Texture* texture);

private:
	// Default texture fallback
	static uint32_t defaultTextureId;

	// Texture type
	TextureType type;

	// Path of texture source
	std::string path;

	// Dynamic temporary texture data
	unsigned char* data;

	uint32_t width;
	uint32_t height;
	uint32_t channels;

	// Backend id of texture
	uint32_t id;
};