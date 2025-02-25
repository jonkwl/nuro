#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <viewport/viewport.h>
#include <rendering/postprocessing/post_processing.h>
#

class Shader;

class VelocityBuffer
{
public:
	explicit VelocityBuffer(const Viewport& viewport);

	void create();	// Setup velocity buffer
	void destroy(); // Delete velocity buffer

	uint32_t render(const glm::mat4& view, const glm::mat4& projection, const PostProcessing::Profile& profile); // Renders the velocity buffer and returns the filtered output

private:
	uint32_t velocityPass(const glm::mat4& view, const glm::mat4& projection);	  // Performs velocity passes to render velocity buffer and returns velocity buffer
	uint32_t postfilteringPass(); // Performs postfiltering pass on rendered velocity buffer and returns postfiltered velocity buffer

private:
	const Viewport& viewport;

	uint32_t fbo; // Framebuffer
	uint32_t rbo; // Renderbuffer

	uint32_t output;			 // Rendered velocity buffer
	uint32_t postfilteredOutput; // Postfilter passed velocity buffer
	// Postfilter applies morphological dilation on velocity buffer to decrease silhouettes

	Shader* velocityPassShader; // Shader for velocity pass
	Shader* postfilterShader; // Shader for performing postfilter pass on velocity buffer
};
