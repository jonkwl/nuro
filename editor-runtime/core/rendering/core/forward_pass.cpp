#include "forward_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/old_entity/old_entity.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/ecs/ecs.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/rendering/core/transformation.h"

#include "../src/runtime/runtime.h"

ForwardPass::ForwardPass(const Viewport& viewport) : clearColor(glm::vec4(0.0f)),
viewport(viewport),
skybox(nullptr),
imGizmo(nullptr),
outputFbo(0),
outputColor(0),
outputDepth(0),
multisampledFbo(0),
multisampledRbo(0),
multisampledColorBuffer(0),
defaultMaterial(nullptr)
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
		Log::printError("Framebuffer", "Error generating forward pass output framebuffer: " + std::to_string(fboStatus));
	}

	// Generate multisampled framebuffer
	glGenFramebuffers(1, &multisampledFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Generate multisampled color buffer texture
	glGenTextures(1, &multisampledColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA16F, viewport.width, viewport.height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorBuffer, 0);

	// Generate multisampled depth buffer
	glGenRenderbuffers(1, &multisampledRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH_COMPONENT24, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, multisampledRbo);

	// Check for multisampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating forward pass multisampled framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create default material
	defaultMaterial = new LitMaterial();
	defaultMaterial->baseColor = glm::vec4(glm::vec3(0.75f), 1.0f);
	defaultMaterial->roughness = 0.3f;
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

unsigned int ForwardPass::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Clear framebuffer
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport
	glViewport(0, 0, viewport.width, viewport.height);

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

	// Render each entity
	auto targets = ECS::getRegistry().view<TransformComponent, MeshRendererComponent>();
	for (auto [entity, transform, renderer] : targets.each()) {
		// Transform components model and mvp must have been calculated beforehand

		// Bind mesh
		glBindVertexArray(renderer.mesh.getVAO());

		// Bind material
		IMaterial* material = defaultMaterial; // implement new material system here
		material->bind();

		// Set shader uniforms
		Shader* shader = material->getShader();
		shader->setMatrix4("mvpMatrix", transform.mvp);
		shader->setMatrix4("modelMatrix", transform.model);
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(transform.model));
		shader->setMatrix3("normalMatrix", normalMatrix);
		// Handle light space

		// Render mesh
		glDrawElements(GL_TRIANGLES, renderer.mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
	}

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (skybox) skybox->render(view, projection);
	glDepthFunc(GL_LESS);

	// Render quick gizmos
	if (imGizmo) {
		imGizmo->render(viewProjection);
	}

	// Bilt multisampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFbo);
	glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return outputColor;
}

unsigned int ForwardPass::getDepthOutput()
{
	return outputDepth;
}

void ForwardPass::setSkybox(Skybox* _skybox)
{
	skybox = _skybox;
}

void ForwardPass::disableSkybox()
{
	skybox = nullptr;
}

void ForwardPass::enableQuickGizmo(IMGizmo* source)
{
	imGizmo = source;
}

void ForwardPass::disableQuickGizmo()
{
	imGizmo = nullptr;
}

void ForwardPass::setClearColor(glm::vec4 _clearColor)
{
	clearColor = _clearColor;
}
