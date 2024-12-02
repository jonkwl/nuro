#pragma once

#include <vector>

#include "../core/viewport/viewport.h"

class Shader;
class Entity;

class VelocityBuffer
{
public:
	VelocityBuffer(const Viewport& viewport);

	void create();	// Setup velocity buffer
	void destroy(); // Delete velocity buffer

	unsigned int render(std::vector<Entity*>& targets); // Renders the velocity buffer and returns the filtered output

private:
	unsigned int velocityPasses(std::vector<Entity*>& targets);	  // Performs velocity passes to render velocity buffer and returns velocity buffer
	unsigned int postfilteringPass(); // Performs postfiltering pass on rendered velocity buffer and returns postfiltered velocity buffer

private:
	const Viewport& viewport;

	unsigned int fbo; // Framebuffer
	unsigned int rbo; // Renderbuffer

	unsigned int output;			 // Rendered velocity buffer
	unsigned int postfilteredOutput; // Postfilter passed velocity buffer
	// Postfilter applies morphological dilation on velocity buffer to decrease silhouettes

	Shader* velocityPassShader; // Shader for velocity pass
	Shader* postfilterShader; // Shader for performing postfilter pass on velocity buffer
};
