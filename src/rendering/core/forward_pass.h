#pragma once

#include <glad/glad.h>

class ForwardPass
{
public:
	static void setup(unsigned int msaaSamples);
	static unsigned int render();
private:
	static unsigned int output; // Output texture

	static unsigned int framebuffer; // Forward pass framebuffer
	static unsigned int rbo; // Forward pass renderbuffer

	static unsigned int msaaFbo; // Anti-Aliasing framebuffer
	static unsigned int msaaRbo; // Anti-Aliasing renderbuffer
	static unsigned int msaaColorBuffer; // Anti-Aliasing color buffer texture
};