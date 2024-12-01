#include "motion_blur_pass.h"

#include <glad/glad.h>

#include "../src/runtime/runtime.h"
#include "../src/utils/log.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/diagnostics/diagnostics.h"

MotionBlurPass::MotionBlurPass(Viewport& viewport) : viewport(viewport),
fbo(0),
output(0),
shader(nullptr),
previousViewProjectionMatrix(glm::mat4(1.0f)),
velocityBuffer(viewport)
{
}

void MotionBlurPass::create()
{
	// Get motion blur pass shader
	shader = ShaderPool::get("motion_blur_pass");

	// Set static shader uniforms
	shader->bind();

	shader->setInt("hdrInput", HDR_UNIT);
	shader->setInt("depthInput", DEPTH_UNIT);
	shader->setInt("velocityInput", VELOCITY_UNIT);

	shader->setFloat("near", 0.3f);
	shader->setFloat("far", 1000.0f);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.width, viewport.height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Motion Blur Pass", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup velocity buffer
	velocityBuffer.create();
}

void MotionBlurPass::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Destroy velocity buffer
	velocityBuffer.destroy();

	// Remove shader
	shader = nullptr;
}

unsigned int MotionBlurPass::render(unsigned int hdrInput, unsigned int depthInput)
{
	// Render velocity buffer if object motion blur is active
	unsigned int VELOCITY_BUFFER = 0;

	bool objectEnabled = PostProcessing::motionBlur.objectEnabled;
	if (objectEnabled)
	{
		VELOCITY_BUFFER = velocityBuffer.render();
	}

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind textures
	glActiveTexture(GL_TEXTURE0 + HDR_UNIT);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	glActiveTexture(GL_TEXTURE0 + DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	glActiveTexture(GL_TEXTURE0 + VELOCITY_UNIT);
	glBindTexture(GL_TEXTURE_2D, VELOCITY_BUFFER);

	// Bind shader
	shader->bind();

	// Set shader uniforms
	shader->setFloat("fps", Diagnostics::getFps());

	bool cameraEnabled = PostProcessing::motionBlur.cameraEnabled;
	shader->setBool("camera", cameraEnabled);
	if (cameraEnabled)
	{
		shader->setFloat("cameraIntensity", PostProcessing::motionBlur.cameraIntensity);
		shader->setInt("cameraSamples", PostProcessing::motionBlur.cameraSamples);
	}

	shader->setBool("object", objectEnabled);
	if (objectEnabled)
	{
		shader->setInt("objectSamples", PostProcessing::motionBlur.objectSamples);
	}

	shader->setMatrix4("inverseViewMatrix", glm::inverse(MeshRenderer::currentViewMatrix));
	shader->setMatrix4("inverseProjectionMatrix", glm::inverse(MeshRenderer::currentProjectionMatrix));
	shader->setMatrix4("previousViewProjectionMatrix", previousViewProjectionMatrix);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Cache current view projection matrix
	previousViewProjectionMatrix = MeshRenderer::currentViewProjectionMatrix;

	// Return output
	return output;
}
