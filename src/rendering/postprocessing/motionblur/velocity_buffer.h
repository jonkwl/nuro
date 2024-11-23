#pragma once

#include <glm.hpp>
#include <glad/glad.h>
#include <vector>

class VelocityBuffer
{
public:
	static void setup();
	static unsigned int render();
private:
	static unsigned int fbo;
	static unsigned int rbo;
	static unsigned int output;
};
