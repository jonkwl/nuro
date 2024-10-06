#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include "../engine/utils/log.h"

enum TextureType {
	DIFFUSE,
	NORMAL,
	METALLIC
};

class Texture
{
public:
	Texture(std::string path, TextureType type = DIFFUSE);

	TextureType type;

	void bind(unsigned int slot);
private:
	unsigned int id;
};