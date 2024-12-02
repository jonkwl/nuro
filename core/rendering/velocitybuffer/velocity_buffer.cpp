#include "velocity_buffer.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../core/utils/log.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/entity/entity.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/postprocessing/post_processing.h"

VelocityBuffer::VelocityBuffer(const Viewport& viewport) : viewport(viewport),
fbo(0),
rbo(0),
output(0),
postfilteredOutput(0),
velocityPassShader(nullptr),
postfilterShader(nullptr)
{
}

void VelocityBuffer::create()
{
	// Get shaders
	velocityPassShader = ShaderPool::get("velocity_pass");
	postfilterShader = ShaderPool::get("velocity_postfilter");

	// Set shaders static uniforms
	postfilterShader->setInt("velocityBuffer", 0);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	// RED CHANNEL = x velocity | GREEN CHANNEL = y velocity | BLUE CHANNEL = view space depth
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Set output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Generate postfiltered output texture
	glGenTextures(1, &postfilteredOutput);
	glBindTexture(GL_TEXTURE_2D, postfilteredOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Set postfiltered output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create depth buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Velocity Buffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VelocityBuffer::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete postfiltered output texture
	glDeleteTextures(1, &postfilteredOutput);
	postfilteredOutput = 0;

	// Delete renderbuffer
	glDeleteRenderbuffers(1, &rbo);
	rbo = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Remove shaders
	velocityPassShader = nullptr;
	postfilterShader = nullptr;
}

unsigned int VelocityBuffer::render(std::vector<Entity*>& targets)
{
	// Prepare output
	unsigned int OUTPUT = 0;

	// Render velocity buffer
	OUTPUT = velocityPasses(targets);

	// Perform postfiltering pass on velocity buffer if object silhouettes should be extended
	if (PostProcessing::motionBlur.objectSilhouetteExtension)
	{
		OUTPUT = postfilteringPass();
	}

	// Return final output
	return OUTPUT;
}

unsigned int VelocityBuffer::velocityPasses(std::vector<Entity*>& targets)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Set render target to output texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Clear framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Bind shader
	velocityPassShader->bind();

	// Render velocity buffer by performing velocity pass on each object
	for (int i = 0; i < targets.size(); i++)
	{
		targets[i]->meshRenderer.velocityPass(velocityPassShader);
	}

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);

	// Return output
	return output;
}

unsigned int VelocityBuffer::postfilteringPass()
{
	// Set render target to postfiltered output texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postfilteredOutput, 0);

	// Bind postfilter shader
	postfilterShader->bind();
	postfilterShader->setVec2("resolution", glm::vec2(viewport.width, viewport.height));

	// Bind velocity buffer texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, output);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	return postfilteredOutput;
}