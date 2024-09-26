#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

#include "../engine/utils/log/log.h"

class Texture
{
public:
	Texture(std::string path, bool& uploaded);

	void use();
private:
	unsigned int id;
};