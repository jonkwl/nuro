#include "forward_pass.h"

#include "../src/runtime/runtime.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"
#include "../src/rendering/gizmos/quick_gizmo.h"

unsigned int ForwardPass::output = 0;

unsigned int ForwardPass::framebuffer = 0;
unsigned int ForwardPass::rbo = 0;

unsigned int ForwardPass::msaaFbo = 0;
unsigned int ForwardPass::msaaRbo = 0;
unsigned int ForwardPass::msaaColorBuffer = 0;

void ForwardPass::setup(unsigned int msaaSamples)
{
	// Initialize parameters needed
	int width = Window::width;
	int height = Window::height;

	// Create forward pass framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create forward pass framebuffer texture
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Create forward pass render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check for forward pass framebuffer error
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
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA16F, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer, 0);

	// Create multi-sampled renderbuffer for depth and stencil
	glGenRenderbuffers(1, &msaaRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, msaaRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaRbo);

	// Check for multi-sampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int ForwardPass::render()
{
	// Initialize parameters needed
	bool wireframe = Runtime::wireframe;

	int width = Window::width;
	int height = Window::height;

	std::vector<Entity*> entityLinks = Runtime::entityLinks;

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);

	// Set viewport and bind post processing framebuffer
	if (!wireframe) {
		glm::vec4 clearColor = Runtime::clearColor;
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	}
	else {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	// Set wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set culling to back face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	// INJECTED PRE PASS START
	
	// Disable color writing
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	// Bind pre pass shader
	Runtime::prePassShader->bind();
	// Pre pass render each entity
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->prePass();
	}
	// Re-enable color writing after pre pass
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// GL_EQUAL depth testing for upcoming forward pass to use pre pass depth data
	glDepthFunc(GL_EQUAL);

	// INJECTED PRE PASS END


	// Render each linked entity to bound forward pass frame
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->forwardPass();
	}

	// Disable wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	Skybox* activeSkybox = Runtime::activeSkybox;
	if (activeSkybox != nullptr && !wireframe) {
		activeSkybox->render(MeshRenderer::currentViewMatrix, MeshRenderer::currentProjectionMatrix);
	}
	glDepthFunc(GL_LESS);

	// Render quick gizmos
	QuickGizmo::render();

	// Bilt multi-sampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return output;
}