#include "debug_pass.h"

#include "../src/rendering/primitives/quad.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"

unsigned int DebugPass::fbo = 0;
unsigned int DebugPass::output = 0;

Shader* DebugPass::shader = nullptr;

void DebugPass::setup()
{
	// Get debug pass shader
	shader = ShaderPool::get("debug_pass");

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating fbo: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int DebugPass::render(unsigned int hdrInput)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind input texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	// Bind debug pass shader
	shader->bind();

	// Set debug pass shader uniforms
	shader->setInt("inputTexture", 0);
	shader->setVec2("resolution", Window::getSize());

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Return output
	return output;
}