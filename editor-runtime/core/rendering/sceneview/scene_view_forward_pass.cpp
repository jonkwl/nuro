#include "scene_view_forward_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/entity/entity.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/rendering/skybox/skybox.h"

SceneViewForwardPass::SceneViewForwardPass(const Viewport& viewport) : wireframe(false),
clearColor(glm::vec4(0.0f)),
viewport(viewport),
drawSkybox(false),
skybox(nullptr),
quickGizmo(nullptr),
fbo(0),
rbo(0),
outputColor(0)
{
}

void SceneViewForwardPass::create()
{
	// Generate forward pass framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate color output texture
	glGenTextures(1, &outputColor);
	glBindTexture(GL_TEXTURE_2D, outputColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.width, viewport.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputColor, 0);

	// Generate renderbuffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check for forward pass framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneViewForwardPass::destroy() {
	// Delete color output texture
	glDeleteTextures(1, &outputColor);
	outputColor = 0;

	// Delete renderbuffer
	glDeleteRenderbuffers(1, &rbo);
	rbo = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;
}

unsigned int SceneViewForwardPass::render(std::vector<Entity*>& targets)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

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

	// Render each linked entity to bound forward pass frame
	for (int i = 0; i < targets.size(); i++) targets[i]->meshRenderer.forwardPass();

	// Disable wireframe if enabled
	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Disable culling before rendering skybox
	glDisable(GL_CULL_FACE);

	// Render skybox to bound forward pass frame
	glDepthFunc(GL_LEQUAL);
	if (drawSkybox && skybox) skybox->render(MeshRenderer::currentViewMatrix, MeshRenderer::currentProjectionMatrix);
	glDepthFunc(GL_LESS);

	// Render quick gizmos
	if (quickGizmo) quickGizmo->render();

	return outputColor;
}

void SceneViewForwardPass::setSkybox(Skybox* source)
{
	skybox = source;
}

void SceneViewForwardPass::enableQuickGizmo(QuickGizmo* source)
{
	quickGizmo = source;
}

void SceneViewForwardPass::disableQuickGizmo()
{
	quickGizmo = nullptr;
}
