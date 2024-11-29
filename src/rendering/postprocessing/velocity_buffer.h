#pragma once

class Shader;

class VelocityBuffer
{
public:
	VelocityBuffer();

	void create();		  // Setup velocity buffer
	void destroy(); // Delete velocity buffer

	unsigned int render(); // Renders the velocity buffer and returns the filtered output

private:
	unsigned int velocityPasses();	 // Performs velocity passes to render velocity buffer and returns velocity buffer
	unsigned int postfilteringPass(); // Performs postfiltering pass on rendered velocity buffer and returns postfiltered velocity buffer

private:
	bool created;

	unsigned int fbo; // Framebuffer
	unsigned int rbo; // Renderbuffer

	unsigned int output;				// Rendered velocity buffer
	unsigned int postfilteredOutput; // Postfilter passed velocity buffer
	// Postfilter applies morphological dilation on velocity buffer to decrease silhouettes

	Shader *postfilterShader; // Shader for performing postfilter pass on velocity buffer
};
