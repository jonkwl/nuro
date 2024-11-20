#pragma once

#include <glad/glad.h>

class ForwardPass
{
public:
	static void setup(unsigned int msaaSamples);
	static unsigned int render(); // Returns color output

	static unsigned int getDepthOutput(); // Returns depth output
private:
	static unsigned int outputFbo; // Output framebuffer
	static unsigned int outputColor; // Output texture
	static unsigned int outputDepth; // Output texture

	static unsigned int multisampledFbo; // Anti-Aliasing framebuffer
	static unsigned int multisampledRbo; // Anti-Aliasing renderbuffer
	static unsigned int multisampledColorBuffer; // Anti-Aliasing color buffer texture
};