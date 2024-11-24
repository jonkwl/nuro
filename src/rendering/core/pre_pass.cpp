#include "pre_pass.h"

#include "../src/runtime/runtime.h"

unsigned int PrePass::width = 0;
unsigned int PrePass::height = 0;

unsigned int PrePass::output = 0;
unsigned int PrePass::fbo = 0;

void PrePass::setup(unsigned int width, unsigned int height)
{
	// Initialize members
	PrePass::width = width;
	PrePass::height = height;

	PrePass::output = 0;
	PrePass::fbo = 0;

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);

	// Generate depth output
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Set depth output parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind framebuffer and set depth output as rendering target
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, output, 0);

	// Deactivate framebuffer draw and read buffers
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Check for framebuffer errors
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating bloom fbo: " + std::to_string(fboStatus));
	}

	// Unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int PrePass::render()
{
	// Set viewport for upcoming pre pass
	glViewport(0, 0, width, height);

	// Bind pre pass framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Cull backfaces
	glCullFace(GL_BACK);

	// Disable color writing
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Bind pre pass shader
	Runtime::prePassShader->bind();

	// Pre pass render each entity
	std::vector<Entity*> entityLinks = Runtime::entityLinks;
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->prePass();
	}

	// Re-enable color writing after pre pass
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Return pre pass output
	return output;
}

unsigned int PrePass::getOutput()
{
	// Return pre pass output
	return output;
}

unsigned int PrePass::getWidth()
{
	// Return width
	return width;
}

unsigned int PrePass::getHeight()
{
	// Return height
	return height;
}

unsigned int PrePass::getFramebuffer()
{
	// Return fbo
	return fbo;
}