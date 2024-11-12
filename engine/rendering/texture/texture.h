#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include "../engine/utils/log.h"

enum TextureType {
	ALBEDO_MAP,
	NORMAL_MAP,
	ROUGHNESS_MAP,
	METALLIC_MAP,
	AMBIENT_OCCLUSION_MAP,
	EMISSION_MAP
};

class Texture
{
public:
	Texture(std::string path, TextureType type);

	TextureType type;

	void bind(unsigned int slot);
private:
	unsigned int id;
};