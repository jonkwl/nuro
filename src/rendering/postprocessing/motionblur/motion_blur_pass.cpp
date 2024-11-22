#include "motion_blur_pass.h"

#include "../src/runtime/runtime.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/core/mesh_renderer.h"

unsigned int MotionBlurPass::fbo = 0;
unsigned int MotionBlurPass::output = 0;

Shader* MotionBlurPass::shader = nullptr;

glm::mat4 MotionBlurPass::previousViewProjectionMatrix = glm::mat4(1.0f);

void MotionBlurPass::setup()
{
	// Get motion blur pass shader
	shader = ShaderPool::get("motion_blur_pass");

	// Set static shader uniforms
	shader->bind();

	shader->setInt("hdrInput", 0);
	shader->setInt("depthInput", 1);

	shader->setFloat("near", 0.3f);
	shader->setFloat("far", 1000.0f);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate output texture
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating post processing framebuffer: " + std::to_string(fboStatus));
	}
}

unsigned int MotionBlurPass::render(unsigned int hdrInput, unsigned int depthInput)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Get current matrices
	glm::mat4 currentViewProjectionMatrix = MeshRenderer::currentViewProjectionMatrix;

	// Bind shader
	shader->bind();
	
	// Set shader uniforms
	shader->setFloat("fps", Runtime::fps);

	shader->setBool("camera", PostProcessing::configuration.motionBlurCamera);
	shader->setFloat("cameraIntensity", PostProcessing::configuration.motionBlurCameraIntensity);
	shader->setInt("cameraSamples", PostProcessing::configuration.motionBlurCameraSamples);

	shader->setMatrix4("previousViewProjectionMatrix", previousViewProjectionMatrix);
	shader->setMatrix4("inverseViewProjectionMatrix", glm::inverse(currentViewProjectionMatrix));

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Cache current view projection matrix
	previousViewProjectionMatrix = currentViewProjectionMatrix;

	// Return output
	return output;
}
