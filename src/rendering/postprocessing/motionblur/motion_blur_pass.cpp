#include "motion_blur_pass.h"

#include "../src/runtime/runtime.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/postprocessing/motionblur/velocity_buffer.h"

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

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup velocity buffer
	VelocityBuffer::setup();
}

unsigned int MotionBlurPass::render(unsigned int hdrInput, unsigned int depthInput)
{
	// Render velocity buffer if object motion blur is active
	unsigned int VELOCITY_BUFFER = 0;
	if (PostProcessing::configuration.motionBlurObject) {
		VELOCITY_BUFFER = VelocityBuffer::render();
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
	shader->setFloat("fps", Runtime::fps);

	bool camera = PostProcessing::configuration.motionBlurCamera;
	shader->setBool("camera", camera);
	if (camera) {
		shader->setFloat("cameraIntensity", PostProcessing::configuration.motionBlurCameraIntensity);
		shader->setInt("cameraSamples", PostProcessing::configuration.motionBlurCameraSamples);
	}

	bool object = PostProcessing::configuration.motionBlurObject;
	shader->setBool("object", object);
	if (object) {
		shader->setInt("objectSamples", PostProcessing::configuration.motionBlurObjectSamples);
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
