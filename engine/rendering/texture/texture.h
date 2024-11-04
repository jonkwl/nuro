#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include "../engine/utils/log.h"

enum TextureType {
	ALBEDO,
	NORMAL,
	ROUGHNESS,
	METALLIC,
	AMBIENT_OCCLUSION
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