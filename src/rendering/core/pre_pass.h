#pragma once

#include "../src/rendering/shader/Shader.h"

class PrePass
{
public:
	static void setup(unsigned int width, unsigned int height);

	static void render();
	static void bind(unsigned int unit);

	static unsigned int getWidth();
	static unsigned int getHeight();
	static unsigned int getFramebuffer();
private:
	static unsigned int width;
	static unsigned int height;

	static unsigned int texture;
	static unsigned int framebuffer;
};

