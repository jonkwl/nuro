#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../src/rendering/shader/Shader.h"

class ShadowMap
{
public:
	ShadowMap(unsigned int size);

	void render();
	void bind(unsigned int slot);

	unsigned int getSize() const;
	unsigned int getFramebuffer() const;
private:
	unsigned int size;

	unsigned int texture;
	unsigned int framebuffer;
};