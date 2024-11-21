#pragma once

#include "../src/rendering/shader/Shader.h"

class PrePass
{
public:
	static void setup(unsigned int width, unsigned int height);

	static unsigned int render();
	static unsigned int getOutput();

	static unsigned int getWidth();
	static unsigned int getHeight();
	static unsigned int getFramebuffer();
private:
	static unsigned int fbo;
	static unsigned int output;

	static unsigned int width;
	static unsigned int height;
};

