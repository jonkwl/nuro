#include "scene_view_forward_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/transform/transform.h"
#include "../core/rendering/model/mesh.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/transformation/transformation.h"
#include "../core/rendering/material/unlit/unlit_material.h"

#include "../src/gizmos/component_gizmos.h"

SceneViewForwardPass::SceneViewForwardPass(const Viewport& viewport) : wireframe(false),
clearColor(glm::vec4(0.0f)),
viewport(viewport),
drawSkybox(false),
drawGizmos(true),
skybox(nullptr),
gizmos(nullptr),
outputFbo(0),
outputColor(0),
multisampledFbo(0),
multisampledRbo(0),
multisampledColorBuffer(0),
selectionMaterial(nullptr)
{
}

void SceneViewForwardPass::create(uint32_t msaaSamples)
{
	// Create outline material
	selectionMaterial = new UnlitMaterial();
	selectionMaterial->baseColor = glm::vec4(1.0f, 0.1f, 0.04f, 1.0f);

	// Generate forward pass framebuffer
	glGenFramebuffers(1, &outputFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);

	// Generate color output texture
	glGenTextures(1, &outputColor);
	glBindTexture(GL_TEXTURE_2D, outputColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.getWidth_gl(), viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);
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
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA16F, viewport.getWidth_gl(), viewport.getHeight_gl(), GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledColorBuffer, 0);

	// Generate multisampled depth buffer
	glGenRenderbuffers(1, &multisampledRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, multisampledRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, viewport.getWidth_gl(), viewport.getHeight_gl());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, multisampledRbo);

	// Check for multisampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating scene view pass multisampled framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

uint32_t SceneViewForwardPass::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const Camera& camera, const std::vector<EntityContainer*>& selectedEntities)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, multisampledFbo);

	// Clear framebuffer
	if (!wireframe)
	{
		// Use default clear color
		glClearColor(defaultClearColor[0], defaultClearColor[1], defaultClearColor[2], 1.0f);
	}
	else
	{
		// Use white as clear color if wireframe
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Set viewport
	glViewport(0, 0, viewport.getWidth_gl(), viewport.getHeight_gl());

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

	// Set wireframe if enabled
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

	// Render each entity
	renderMeshes(selectedEntities);

	// Render selected entity with outline
	for (auto& entity : selectedEntities) {
		renderSelectedEntity(entity, viewProjection, camera);
	}

	// Disable wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (drawSkybox && skybox) skybox->render(view, projection);
	glDepthFunc(GL_LESS);

	// Render gizmos
	if (drawGizmos && gizmos) gizmos->renderAll(viewProjection);

	// Disable stencil testing
	glDisable(GL_STENCIL_TEST);

	// Bilt multisampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, outputFbo);
	glBlitFramebuffer(0, 0, viewport.getWidth_gl(), viewport.getHeight_gl(), 0, 0, viewport.getWidth_gl(), viewport.getHeight_gl(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return outputColor;
}

void SceneViewForwardPass::linkSkybox(Skybox* source)
{
	skybox = source;
}

void SceneViewForwardPass::linkGizmos(IMGizmo* _gizmos)
{
	gizmos = _gizmos;
}

void SceneViewForwardPass::renderMesh(TransformComponent& transform, MeshRendererComponent& renderer)
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

void SceneViewForwardPass::renderMeshes(const std::vector<EntityContainer*>& skippedEntities)
{
	uint32_t currentShaderId = 0;
	uint32_t currentMaterialId = 0;

	uint16_t newBoundShaders = 0;
	uint16_t newBoundMaterials = 0;

	// Render each entity except for skipped one
	for (auto& [entity, transform, renderer] : ECS::getRenderQueue()) {

		// Skip if target entity is selected entity
		// tmp
		if (skippedEntities.size() > 0) {
			if (skippedEntities[0]->root == entity) continue;
		}

		uint32_t shaderId = renderer.material->getShaderId();
		if (shaderId != currentShaderId) {
			renderer.material->getShader()->bind();
			currentShaderId = shaderId;
			newBoundShaders++;
		}

		uint32_t materialId = renderer.material->getId();
		if (materialId != currentMaterialId) {
			renderer.material->bind();
			currentMaterialId = materialId;
			newBoundMaterials++;
		}

		renderMesh(transform, renderer);

	}

	// Log::printProcessInfo("New bound - Shaders / Materials : " + std::to_string(newBoundShaders) + " / " + std::to_string(newBoundMaterials));
}

void SceneViewForwardPass::renderSelectedEntity(EntityContainer* entity, const glm::mat4& viewProjection, const Camera& camera)
{
	// Make sure selected entity is renderable
	if (!entity->has<MeshRendererComponent>()) return;

	// Fetch components
	TransformComponent& transform = entity->transform;
	MeshRendererComponent& renderer = entity->get<MeshRendererComponent>();

	// Render selected entitites gizmos if needed
	if (gizmos) ComponentGizmos::renderEntityGizmos(*gizmos, *entity);

	// Don't render outline if wireframe is enabled
	if (wireframe) return;

	// Render the selected entity and write to stencil
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass, write 1 to stencil buffer
	glStencilMask(0xFF); // Enable stencil writes

	// Forward render entities base mesh
	Shader* shader = renderer.material->getShader();
	shader->bind();
	shader->setMatrix4("mvpMatrix", transform.mvp);
	shader->setMatrix4("modelMatrix", transform.model);
	shader->setMatrix3("normalMatrix", transform.normal);
	renderer.material->bind();
	glBindVertexArray(renderer.mesh.getVAO());
	glDrawElements(GL_TRIANGLES, renderer.mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);

	// Render outline of selected entity
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Pass if stencil value is NOT 1
	glStencilMask(0x00); // Disable stencil writes
	glDepthFunc(GL_LEQUAL); // Pass depth test if less or equal

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get outline thickness according to camera distance
	float distance = glm::distance(camera.transform.position, transform.position);
	float baseThickness = 0.038f;
	float thickness = baseThickness;

	// Temporary transform component
	TransformComponent outlineTransform;
	outlineTransform.position = transform.position;
	outlineTransform.rotation = transform.rotation;
	outlineTransform.scale = transform.scale + thickness;
	outlineTransform.parent = transform.parent;
	Transform::evaluate(outlineTransform, viewProjection);

	// Render mesh as outline
	shader = selectionMaterial->getShader();
	shader->bind();
	shader->setMatrix4("mvpMatrix", outlineTransform.mvp);
	shader->setMatrix4("modelMatrix", outlineTransform.model);
	shader->setMatrix3("normalMatrix", outlineTransform.normal);
	selectionMaterial->bind();
	glBindVertexArray(renderer.mesh.getVAO());
	glDrawElements(GL_TRIANGLES, renderer.mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);

	// Reset state
	glDisable(GL_BLEND);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
}