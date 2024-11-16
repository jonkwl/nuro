#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../src/rendering/shader/Shader.h"

class ShadowMap
{
public:
	ShadowMap(unsigned int width, unsigned int height);

	void render();
	void bind(unsigned int unit);

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned int getFramebuffer() const;
private:
	unsigned int width;
	unsigned int height;

	unsigned int texture;
	unsigned int framebuffer;
};