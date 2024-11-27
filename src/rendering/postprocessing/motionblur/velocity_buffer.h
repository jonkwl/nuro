#pragma once

class Shader;

class VelocityBuffer
{
public:
	static void setup();		  // Setup velocity buffer
	static unsigned int render(); // Renders the velocity buffer and returns the filtered output
private:
	static unsigned int fbo; // Framebuffer
	static unsigned int rbo; // Renderbuffer

	static unsigned int output;				// Rendered velocity buffer
	static unsigned int postfilteredOutput; // Postfilter passed velocity buffer
	// Postfilter applies morphological dilation on velocity buffer to decrease silhouettes

	static Shader *postfilterShader; // Shader for performing postfilter pass on velocity buffer

	static unsigned int velocityPasses();	 // Performs velocity passes to render velocity buffer and returns velocity buffer
	static unsigned int postfilteringPass(); // Performs postfiltering pass on rendered velocity buffer and returns postfiltered velocity buffer
};
