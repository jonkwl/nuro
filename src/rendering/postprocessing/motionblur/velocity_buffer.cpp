#include "velocity_buffer.h"

#include "../src/window/window.h"
#include "../src/runtime/runtime.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/shader/Shader.h"
#include "../src/rendering/primitives/quad.h"

unsigned int VelocityBuffer::fbo = 0;
unsigned int VelocityBuffer::rbo = 0;

unsigned int VelocityBuffer::output = 0;
unsigned int VelocityBuffer::postfilteredOutput = 0;

Shader* VelocityBuffer::postfilterShader = nullptr;

void VelocityBuffer::setup()
{
	// Get postfilter shader
	postfilterShader = ShaderPool::get("velocity_postfilter");

	// Set postfilter static uniforms
	postfilterShader->setInt("velocityBuffer", 0);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	// RED CHANNEL = x velocity | GREEN CHANNEL = y velocity | BLUE CHANNEL = view space depth
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::width, Window::height, 0, GL_RGB, GL_FLOAT, nullptr);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::width, Window::height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Set postfiltered output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create depth buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating post processing framebuffer: " + std::to_string(fboStatus));
	}
}

unsigned int VelocityBuffer::render()
{
	// Prepare output
	unsigned int OUTPUT = 0;

	// Render velocity buffer
	OUTPUT = velocityPasses();

	// Perform postfiltering pass on velocity buffer if object silhouettes should be extended
	if (PostProcessing::configuration.motionBlurObjectSilhouetteExtension) {
		OUTPUT = postfilteringPass();
	}

	// Return final output
	return OUTPUT;
}

unsigned int VelocityBuffer::velocityPasses()
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
	Runtime::velocityPassShader->bind();

	// Render velocity buffer by performing velocity pass on each object
	std::vector<Entity*> entityLinks = Runtime::entityLinks;
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->velocityPass();
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
	postfilterShader->setVec2("resolution", glm::vec2(Window::width, Window::height));

	// Bind velocity buffer texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, output);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	return postfilteredOutput;
}