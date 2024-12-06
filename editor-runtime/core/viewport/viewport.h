#pragma once

// Functionality may be extended in the future, therefore own header
struct Viewport
{
	
	// Default size of 250x250 to make sure any framebuffer
	// initialized with default viewport doesnt have
	// a width and height of 0x0 (invalid)

	float width = 250.0f;
	float height = 250.0f;
};