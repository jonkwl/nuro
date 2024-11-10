#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../engine/rendering/shader/Shader.h"

class ShadowMap
{
public:
	ShadowMap(unsigned int size);

	void render();
	void bind(unsigned int slot);

	unsigned int getSize();
	unsigned int getFramebuffer();
private:
	unsigned int size;

	unsigned int texture;
	unsigned int framebuffer;
};