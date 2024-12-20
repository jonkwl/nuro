#include "scene_view_forward_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/rendering/model/model.h"
#include "../core/ecs/ecs.h"
#include "../core/rendering/core/transformation.h"

SceneViewForwardPass::SceneViewForwardPass(const Viewport& viewport) : wireframe(false),
clearColor(glm::vec4(0.0f)),
viewport(viewport),
drawSkybox(false),
drawQuickGizmos(true),
skybox(nullptr),
imGizmo(nullptr),
outputFbo(0),
outputColor(0),
multisampledFbo(0),
multisampledRbo(0),
multisampledColorBuffer(0),
selectionMaterial(nullptr),
defaultMaterial(nullptr)
{
}

void SceneViewForwardPass::create(unsigned int msaaSamples)
{
	// Create outline material
	selectionMaterial = new UnlitMaterial();
	selectionMaterial->baseColor = glm::vec4(1.0f, 0.25f, 0.0f, 0.7f);

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
		Log::printError("Framebuffer", "Error generating scene view pass output framebuffer: " + std::to_string(fboStatus));
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
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampledRbo);

	// Check for multisampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating scene view pass multisampled framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create default material
	defaultMaterial = new LitMaterial();
	defaultMaterial->baseColor = glm::vec4(glm::vec3(0.75f), 1.0f);
	defaultMaterial->roughness = 0.3f;
}

void SceneViewForwardPass::destroy() {
	// Delete selection material
	delete(selectionMaterial);
	selectionMaterial = nullptr;

	// Delete color output texture
	glDeleteTextures(1, &outputColor);
	outputColor = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &outputFbo);
	outputFbo = 0;

	// Delete color output texture
	glDeleteTextures(1, &multisampledColorBuffer);
	multisampledColorBuffer = 0;

	// Delete renderbuffer
	glDeleteRenderbuffers(1, &multisampledRbo);
	multisampledRbo = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &multisampledFbo);
	multisampledFbo = 0;
}

unsigned int SceneViewForwardPass::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Clear framebuffer
	if (!wireframe)
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Default scene view clearing outputColor
	}
	else
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Wireframe clearing outputColor
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Set viewport
	glViewport(0, 0, viewport.width, viewport.height);

	// Set wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set culling to back face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable stencil testing without writing
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);

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
	
	// Render selected entity with outline
	// renderSelectedEntity(..., ...);

	// Disable wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (drawSkybox && skybox) skybox->render(view, projection);
	glDepthFunc(GL_LESS);

	// Render quick gizmos
	if (drawQuickGizmos && imGizmo) imGizmo->render(viewProjection);

	// Disable stencil testing
	glDisable(GL_STENCIL_TEST);

	// Bilt multisampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFbo);
	glBlitFramebuffer(0, 0, viewport.width, viewport.height, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return outputColor;
}

void SceneViewForwardPass::setSkybox(Skybox* source)
{
	skybox = source;
}

void SceneViewForwardPass::setQuickGizmo(IMGizmo* source)
{
	imGizmo = source;
}

void SceneViewForwardPass::renderSelectedEntity(TransformComponent& transform, MeshRendererComponent& renderer)
{
	/*
	// Render the selected entity and write to stencil
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass, write 1 to stencil buffer
	glStencilMask(0xFF); // Enable stencil writes
	selected->meshRenderer.forwardPass();

	// Render outline of selected entity
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Pass if stencil value is NOT 1
	glStencilMask(0x00); // Disable stencil writes
	glDepthFunc(GL_LEQUAL); // Pass depth test if less or equal

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Temporarily increase scale for outline rendering
	float scaleIncrease = 0.025f;
	selected->transform.scale += scaleIncrease;

	// Temporarily overwrite all materials with selection material
	std::vector<IMaterial*> originalMaterials = selected->meshRenderer.materials;
	std::fill(selected->meshRenderer.materials.begin(), selected->meshRenderer.materials.end(), selectionMaterial);

	// Recalculate render matrices and render the outline entity
	selected->meshRenderer.recalculateRenderMatrices();
	selected->meshRenderer.forwardPass();

	// Restore original materials, scale and position
	selected->meshRenderer.materials = originalMaterials;
	selected->transform.scale -= scaleIncrease;

	// Reset state
	glDisable(GL_BLEND);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	*/
}