#include "velocity_buffer.h"

#include "../src/window/window.h"
#include "../src/runtime/runtime.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/shader/Shader.h"

unsigned int VelocityBuffer::fbo = 0;
unsigned int VelocityBuffer::rbo = 0;
unsigned int VelocityBuffer::output = 0;

void VelocityBuffer::setup()
{
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
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Clear framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Bind shader
	Runtime::velocityPassShader->bind();

	// Perform velocity pass on each object
	std::vector<Entity*> entityLinks = Runtime::entityLinks;
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->velocityPass();
	}

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);

	// Return output
	return output;
}