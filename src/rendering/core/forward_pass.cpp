#include "forward_pass.h"

#include <glad/glad.h>

#include "../src/utils/log.h"
#include "../src/entity/entity.h"
#include "../src/rendering/core/mesh_renderer.h"

ForwardPass::ForwardPass(const Viewport& viewport) : wireframe(false),
clearColor(glm::vec4(0.0f)),
viewport(viewport),
skybox(nullptr),
quickGizmo(nullptr),
outputFbo(0),
outputColor(0),
outputDepth(0),
multisampledFbo(0),
multisampledRbo(0),
multisampledColorBuffer(0)
{
}

void ForwardPass::create(const unsigned int msaaSamples)
{
	// Generate forward pass framebuffer
	glGenFramebuffers(1, &outputFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);

	// Generate color output texture
	glGenTextures(1, &outputColor);
	glBindTexture(GL_TEXTURE_2D, outputColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.width, viewport.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputColor, 0);

	// Check for forward pass framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Generate multi-sampled framebuffer
	glGenFramebuffers(1, &multisampledFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Generate multi-sampled color buffer texture
	glGenTextures(1, &multisampledColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA16F, viewport.width, viewport.height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorBuffer, 0);

	// Generate multi-sampled depth buffer
	glGenRenderbuffers(1, &multisampledRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH_COMPONENT24, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampledRbo);

	// Check for multi-sampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPass::destroy() {
	// Delete color output texture
	glDeleteTextures(1, &outputColor);
	outputColor = 0;

	// Delete depth output texture
	glDeleteTextures(1, &outputDepth);
	outputDepth = 0;

	// Delete output framebuffer
	glDeleteFramebuffers(1, &outputFbo);
	outputFbo = 0;

	// Delete multisampled color buffer texture
	glDeleteTextures(1, &multisampledColorBuffer);
	multisampledColorBuffer = 0;

	// Delete multisampled renderbuffer
	glDeleteRenderbuffers(1, &multisampledRbo);
	multisampledRbo = 0;

	// Delete multisampled framebuffer
	glDeleteFramebuffers(1, &multisampledFbo);
	multisampledFbo = 0;
}

unsigned int ForwardPass::render(std::vector<Entity*>& targets)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Clear framebuffer
	if (!wireframe)
	{
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	}
	else
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport
	glViewport(0, 0, viewport.width, viewport.height);

	// Set wireframe if enabled
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set culling to back face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// INJECTED PRE PASS START
	/*
	// Disable color writing
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	// Bind pre pass shader
	prePassShader->bind();
	// Pre pass render each entity
	for (int i = 0; i < entityStack.size(); i++) {
		entityStack[i]->meshRenderer->prePass();
	}
	// Re-enable color writing after pre pass
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// GL_EQUAL depth testing for upcoming forward pass to use pre pass depth data
	glDepthFunc(GL_EQUAL);
	*/
	// INJECTED PRE PASS END

	// Render each linked entity to bound forward pass frame
	for (int i = 0; i < targets.size(); i++)
	{
		targets[i]->meshRenderer.forwardPass();
	}

	// Disable wireframe if enabled
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (skybox && !wireframe)
	{
		skybox->render(MeshRenderer::currentViewMatrix, MeshRenderer::currentProjectionMatrix);
	}
	glDepthFunc(GL_LESS);

	// Render quick gizmos
	if (quickGizmo) {
		quickGizmo->render();
	}

	// Bilt multi-sampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFbo);
	glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return outputColor;
}

unsigned int ForwardPass::getDepthOutput()
{
	return outputDepth;
}

void ForwardPass::enableSkybox(Skybox* source)
{
	skybox = source;
}

void ForwardPass::disableSkybox()
{
	skybox = nullptr;
}

void ForwardPass::enableQuickGizmo(QuickGizmo* source)
{
	quickGizmo = source;
}

void ForwardPass::disableQuickGizmo()
{
	quickGizmo = nullptr;
}
