#pragma once

#include "../src/viewport/viewport.h"

class ForwardPass
{
public:
	ForwardPass(Viewport& viewport);

	void create(unsigned int msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	unsigned int render(); // Returns color output

	unsigned int getDepthOutput(); // Returns depth output
private:
	Viewport& viewport;

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output texture
	unsigned int outputDepth; // Output texture

	unsigned int multisampledFbo;		 // Anti-Aliasing framebuffer
	unsigned int multisampledRbo;		 // Anti-Aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-Aliasing color buffer texture
};