#pragma once

#include <glad/glad.h>

class ForwardPassFrame
{
public:
	static void setup(unsigned int msaaSamples);
	static void bind();
	static void render();

	static unsigned int getOutput();
private:
	static unsigned int output; // Output texture

	static unsigned int ppFbo; // Forward pass framebuffer
	static unsigned int ppRbo; // Forward pass renderbuffer

	static unsigned int msaaFbo; // Anti-Aliasing framebuffer
	static unsigned int msaaRbo; // Anti-Aliasing renderbuffer
	static unsigned int msaaColorBuffer; // Anti-Aliasing color buffer texture
};