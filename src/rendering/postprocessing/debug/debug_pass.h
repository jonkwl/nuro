#pragma once

#include <glad/glad.h>

#include "../src/rendering/shader/shader_pool.h"

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

