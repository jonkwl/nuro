#include "motion_blur_pass.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/diagnostics/diagnostics.h"

MotionBlurPass::MotionBlurPass(const Viewport& viewport) : viewport(viewport),
fbo(0),
output(0),
shader(nullptr),
previousViewProjectionMatrix(glm::mat4(1.0f))
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
}

void MotionBlurPass::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Remove shader
	shader = nullptr;
}

unsigned int MotionBlurPass::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const PostProcessing::Profile& profile, const unsigned int hdrInput, const unsigned int depthInput, const unsigned int velocityBufferInput)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind textures
	glActiveTexture(GL_TEXTURE0 + HDR_UNIT);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	glActiveTexture(GL_TEXTURE0 + DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Bind shader
	shader->bind();

	// Set shader uniforms
	shader->setFloat("fps", Diagnostics::getFps());

	bool cameraEnabled = profile.motionBlur.cameraEnabled;
	shader->setBool("camera", cameraEnabled);
	if (cameraEnabled)
	{
		// Set camera motion blur uniforms
		shader->setFloat("cameraIntensity", profile.motionBlur.cameraIntensity);
		shader->setInt("cameraSamples", profile.motionBlur.cameraSamples);
	}

	bool objectEnabled = profile.motionBlur.objectEnabled;
	shader->setBool("object", objectEnabled);
	if (objectEnabled)
	{
		// Attach velocity buffer
		glActiveTexture(GL_TEXTURE0 + VELOCITY_UNIT);
		glBindTexture(GL_TEXTURE_2D, velocityBufferInput);

		// Set object motion blur uniforms
		shader->setInt("objectSamples", profile.motionBlur.objectSamples);
	}

	// Set transformation uniforms
	shader->setMatrix4("inverseViewMatrix", glm::inverse(view));
	shader->setMatrix4("inverseProjectionMatrix", glm::inverse(projection));
	shader->setMatrix4("previousViewProjectionMatrix", previousViewProjectionMatrix);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Cache current view projection matrix
	previousViewProjectionMatrix = viewProjection;

	// Return output
	return output;
}
