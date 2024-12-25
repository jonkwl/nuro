#include "forward_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/model/mesh.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/core/transformation.h"

ForwardPass::ForwardPass(const Viewport& viewport) : drawSkybox(false),
drawGizmos(false),
viewport(viewport),
skybox(nullptr),
gizmos(nullptr),
clearColor(glm::vec4(0.0f)),
outputFbo(0),
outputColor(0),
outputDepth(0),
multisampledFbo(0),
multisampledRbo(0),
multisampledColorBuffer(0)
{
}

void ForwardPass::create(const uint32_t msaaSamples)
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

uint32_t ForwardPass::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection)
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
	for (int32_t i = 0; i < entityStack.size(); i++) {
		entityStack[i]->meshRenderer->prePass();
	}
	// Re-enable color writing after pre pass
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	// GL_EQUAL depth testing for upcoming forward pass to use pre pass depth data
	glDepthFunc(GL_EQUAL);
	*/
	// INJECTED PRE PASS END

	// Render each entity
	renderMeshes();

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (drawSkybox && skybox) skybox->render(view, projection);
	glDepthFunc(GL_LESS);

	// Render gizmos, shapes only
	if (drawGizmos && gizmos) gizmos->renderShapes(viewProjection);

	// Bilt multisampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFbo);
	glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return outputColor;
}

uint32_t ForwardPass::getDepthOutput()
{
	return outputDepth;
}

void ForwardPass::linkSkybox(Skybox* _skybox)
{
	skybox = _skybox;
}

void ForwardPass::linkGizmos(IMGizmo* _gizmos)
{
	gizmos = _gizmos;
}

void ForwardPass::setClearColor(glm::vec4 _clearColor)
{
	clearColor = _clearColor;
}

void ForwardPass::renderMesh(TransformComponent& transform, MeshRendererComponent& renderer)
{
	// Transform components model and mvp must have been calculated beforehand

	// Set shader uniforms
	Shader* shader = renderer.material->getShader();
	shader->setMatrix4("mvpMatrix", transform.mvp);
	shader->setMatrix4("modelMatrix", transform.model);
	shader->setMatrix3("normalMatrix", transform.normal);

	// Bind mesh
	glBindVertexArray(renderer.mesh.getVAO());

	// Render mesh
	glDrawElements(GL_TRIANGLES, renderer.mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
}

void ForwardPass::renderMeshes()
{
	uint32_t currentShaderId = 0;
	uint32_t currentMaterialId = 0;

	// Render each entity
	for (auto& [entity, transform, renderer] : ECS::getRenderQueue()) {

		uint32_t shaderId = renderer.shaderId;
		if (shaderId != currentShaderId) {
			renderer.material->getShader()->bind();
			currentShaderId = shaderId;
		}

		uint32_t materialId = renderer.materialId;
		if (materialId != currentMaterialId) {
			renderer.material->bind();
			currentMaterialId = materialId;
		}

		renderMesh(transform, renderer);

	}
}