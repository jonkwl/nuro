#pragma once

#include <cstdint>
#include <string>

#include <resource/resource.h>

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

class Texture : public Resource
{
protected:
	bool loadIoData() override;
	void freeIoData() override;
	bool uploadBuffers() override;
	void deleteBuffers() override;

public:
	Texture();
	~Texture();

	// Sets the textures type and path of texture source
	void setSource(TextureType type, const std::string& path);

	// Returns the textures backend id
	uint32_t backendId() const;

	// Sets the given texture backend id to be the default backend id for new textures
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
	uint32_t _backendId;
};