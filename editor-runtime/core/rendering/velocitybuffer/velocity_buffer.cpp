#include "velocity_buffer.h"

#include <glad/glad.h>
#include <vector>

#include "../core/utils/log.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/primitives/global_quad.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/model/mesh.h"
#include "../core/ecs/ecs_collection.h"

VelocityBuffer::VelocityBuffer(const Viewport& viewport) : viewport(viewport),
fbo(0),
rbo(0),
output(0),
postfilteredOutput(0),
velocityPassShader(nullptr),
postfilterShader(nullptr)
{
}

void VelocityBuffer::create()
{
	// Get shaders
	velocityPassShader = ShaderPool::get("velocity_pass");
	postfilterShader = ShaderPool::get("velocity_postfilter");

	// Set shaders static uniforms
	postfilterShader->setInt("velocityBuffer", 0);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	// RED CHANNEL = x velocity | GREEN CHANNEL = y velocity | BLUE CHANNEL = view space depth
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Set output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Generate postfiltered output texture
	glGenTextures(1, &postfilteredOutput);
	glBindTexture(GL_TEXTURE_2D, postfilteredOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, nullptr);

	// Set postfiltered output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create depth buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, viewport.width, viewport.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Velocity Buffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VelocityBuffer::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete postfiltered output texture
	glDeleteTextures(1, &postfilteredOutput);
	postfilteredOutput = 0;

	// Delete renderbuffer
	glDeleteRenderbuffers(1, &rbo);
	rbo = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Remove shaders
	velocityPassShader = nullptr;
	postfilterShader = nullptr;
}

uint32_t VelocityBuffer::render(const glm::mat4& view, const glm::mat4& projection, const PostProcessing::Profile& profile)
{
	// Prepare output
	uint32_t OUTPUT = 0;

	// Render velocity buffer
	OUTPUT = velocityPass(view, projection);

	// OUTPUT = postfilteringPass();

	// Return final output
	return OUTPUT;
}

uint32_t VelocityBuffer::velocityPass(const glm::mat4& view, const glm::mat4& projection)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Set render target to output texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Clear framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Bind shader
	velocityPassShader->bind();
	velocityPassShader->setMatrix4("viewMatrix", view);
	velocityPassShader->setMatrix4("projectionMatrix", projection);

	// Render velocity buffer by performing velocity pass on each object
	auto targets = ECS::gRegistry.view<TransformComponent, MeshRendererComponent, VelocityComponent>();
	for (auto [entity, transform, renderer, velocity] : targets.each()) {
		// Bind mesh
		glBindVertexArray(renderer.mesh.getVAO());

		// Set velocity pass shader uniforms
		velocityPassShader->setMatrix4("modelMatrix", transform.model);
		velocityPassShader->setMatrix4("previousModelMatrix", velocity.lastModel);
		velocityPassShader->setFloat("intensity", velocity.intensity);
		  
		// Render mesh
		glDrawElements(GL_TRIANGLES, renderer.mesh.getVerticeCount(), GL_UNSIGNED_INT, 0);

		// Update last model matrix cache
		velocity.lastModel = transform.model;
	}

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);

	// Return output
	return output;
}

uint32_t VelocityBuffer::postfilteringPass()
{
	// Set render target to postfiltered output texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postfilteredOutput, 0);

	// Bind postfilter shader
	postfilterShader->bind();
	postfilterShader->setVec2("resolution", glm::vec2(viewport.width, viewport.height));

	// Bind velocity buffer texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, output);

	// Bind and render to quad
	GlobalQuad::bind();
	GlobalQuad::render();

	return postfilteredOutput;
}