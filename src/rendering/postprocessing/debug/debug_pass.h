#pragma once

#include <glad/glad.h>

class Shader;

class DebugPass
{
public:
	static void setup(); // Setup debug pass
	static unsigned int render(unsigned int hdrInput); // Render debug pass and return output
private:
	static unsigned int fbo; // Framebuffer
	static unsigned int output; // Output

	static Shader* shader; // Debug pass shader
};

