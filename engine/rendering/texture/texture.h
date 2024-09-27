#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include "../engine/utils/log.h"

class Texture
{
public:
	Texture(std::string path);

	void bind();
private:
	unsigned int id;
};