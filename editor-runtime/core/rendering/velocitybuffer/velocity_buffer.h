#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/postprocessing/post_processing.h"
#

class Shader;
class OldEntity;

class VelocityBuffer
{
public:
	explicit VelocityBuffer(const Viewport& viewport);

	void create();	// Setup velocity buffer
	void destroy(); // Delete velocity buffer

	unsigned int render(const glm::mat4& view, const glm::mat4& projection, const PostProcessing::Profile& profile); // Renders the velocity buffer and returns the filtered output

private:
	unsigned int velocityPass(const glm::mat4& view, const glm::mat4& projection);	  // Performs velocity passes to render velocity buffer and returns velocity buffer
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
