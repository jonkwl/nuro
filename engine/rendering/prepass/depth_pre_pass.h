#pragma once

#include "../engine/rendering/shader/Shader.h"

class DepthPrePass
{
public:
	DepthPrePass(unsigned int width, unsigned int height);

	void render();
	void bind(unsigned int slot);

	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned int getFramebuffer() const;
private:
	unsigned int width;
	unsigned int height;

	unsigned int texture;
	unsigned int framebuffer;
};

