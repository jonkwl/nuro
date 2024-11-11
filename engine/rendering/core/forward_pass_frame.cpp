#include "forward_pass_frame.h"

#include "../engine/window/window.h"
#include "../engine/utils/log.h"

unsigned int ForwardPassFrame::output = 0;

unsigned int ForwardPassFrame::ppFbo = 0;
unsigned int ForwardPassFrame::ppRbo = 0;

unsigned int ForwardPassFrame::msaaFbo = 0;
unsigned int ForwardPassFrame::msaaRbo = 0;
unsigned int ForwardPassFrame::msaaColorBuffer = 0;

void ForwardPassFrame::setup(unsigned int msaaSamples)
{
	// Create post processing framebuffer
	glGenFramebuffers(1, &ppFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ppFbo);

	// Create post processing framebuffer texture
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Create post processing render buffer
	glGenRenderbuffers(1, &ppRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, ppRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ppRbo);

	// Check for post processing framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Create anti-aliasing framebuffer
	glGenFramebuffers(1, &msaaFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);

	// Create multi-sampled color buffer texture
	glGenTextures(1, &msaaColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA16F, Window::width, Window::height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer, 0);

	// Create multi-sampled renderbuffer for depth and stencil
	glGenRenderbuffers(1, &msaaRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, msaaRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaRbo);

	// Check for multi-sampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPassFrame::bind()
{
	// Bind framebuffer
	// glBindFramebuffer(GL_FRAMEBUFFER, ppFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ForwardPassFrame::render()
{
	// Bilt multi-sampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFbo);
	glBlitFramebuffer(0, 0, Window::width, Window::height, 0, 0, Window::width, Window::height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int ForwardPassFrame::getOutput()
{
	return output;
}
