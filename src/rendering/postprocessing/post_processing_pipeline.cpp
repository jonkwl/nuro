#include "post_processing_pipeline.h"

#include <glad/glad.h>
#include <glm.hpp>

#include "../src/utils/log.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/ssao/ssao_pass.h"
#include "../src/rendering/core/pre_pass.h"
#include "../src/rendering/core/forward_pass.h"
#include "../src/rendering/texture/texture.h"
#include "../src/utils/log.h"

PostProcessingPipeline::PostProcessingPipeline(const Viewport& viewport, const bool renderToScreen) : viewport(viewport),
renderToScreen(renderToScreen),
fbo(0),
output(0),
finalPassShader(nullptr),
motionBlurPass(viewport),
bloomPass(viewport)
{
}

void PostProcessingPipeline::create()
{
	// Generate own framebuffer and target textures if post processing pipeline doesnt render to screen
	if (!renderToScreen) {

		// Generate framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Generate output texture
		glGenTextures(1, &output);
		glBindTexture(GL_TEXTURE_2D, output);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.width, viewport.height, 0, GL_RGBA, GL_FLOAT, NULL);

		// Set output texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Attach output texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

		GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::printError("Post Processing Pipeline", "Error generating framebuffer: " + std::to_string(fboStatus));
		}

	}

	// Get default post processing final pass shader
	finalPassShader = ShaderPool::get("final_pass");

	// Bind final pass shader and set static uniforms
	finalPassShader->bind();
	finalPassShader->setInt("hdrBuffer", HDR_UNIT);
	finalPassShader->setInt("depthBuffer", DEPTH_UNIT);
	finalPassShader->setInt("bloomBuffer", BLOOM_UNIT);
	finalPassShader->setInt("configuration.lensDirtTexture", LENS_DIRT_UNIT);

	// Setup post processing pipeline
	motionBlurPass.create();
	bloomPass.create(PostProcessing::bloom.mipDepth);
}

void PostProcessingPipeline::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Destroy all passes
	motionBlurPass.destroy();
	bloomPass.destroy();

	// Remove shader
	finalPassShader = nullptr;
}

void PostProcessingPipeline::render(const unsigned int hdrInput, const unsigned int depthInput, const unsigned int velocityBufferInput)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Pass input through post processing pipeline
	unsigned int POST_PROCESSING_PIPELINE_HDR = hdrInput;

	// Motion blur pass
	if (PostProcessing::motionBlur.enabled)
	{
		// Apply motion blur on post processing hdr input
		POST_PROCESSING_PIPELINE_HDR = motionBlurPass.render(POST_PROCESSING_PIPELINE_HDR, depthInput, velocityBufferInput);
	}

	// Seperate bloom pass
	unsigned int BLOOM_PASS_OUTPUT = 0;
	if (PostProcessing::bloom.enabled)
	{
		bloomPass.threshold = PostProcessing::bloom.threshold;
		bloomPass.softThreshold = PostProcessing::bloom.softThreshold;
		bloomPass.filterRadius = PostProcessing::bloom.filterRadius;
		bloomPass.mipDepth = PostProcessing::bloom.mipDepth;
		BLOOM_PASS_OUTPUT = bloomPass.render(POST_PROCESSING_PIPELINE_HDR);
	}

	// Bind post processing framebuffer (which is 0 if rendering to screen)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setVec2("resolution", glm::vec2(viewport.width, viewport.height));

	// Sync post processing configuration with shader
	syncConfiguration();

	// Bind forward pass hdr color buffer
	glActiveTexture(GL_TEXTURE0 + HDR_UNIT);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_HDR);

	// Bind pre pass depth buffer
	glActiveTexture(GL_TEXTURE0 + DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Bind bloom buffer
	glActiveTexture(GL_TEXTURE0 + BLOOM_UNIT);
	glBindTexture(GL_TEXTURE_2D, BLOOM_PASS_OUTPUT);

	// Bind lens dirt texture
	if (PostProcessing::bloom.lensDirtEnabled)
	{
		PostProcessing::bloom.lensDirtTexture.bind(LENS_DIRT_UNIT);
	}

	// Bind quad and render to screen
	Quad::bind();
	Quad::render();

	// Unbind post processing framebuffer (redundant if rendering to screen)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int PostProcessingPipeline::getOutput()
{
	// Return output of post processing pipeline pass
	return output;
}

void PostProcessingPipeline::syncConfiguration()
{
	finalPassShader->setFloat("configuration.exposure", PostProcessing::color.exposure);
	finalPassShader->setFloat("configuration.contrast", PostProcessing::color.contrast);
	finalPassShader->setFloat("configuration.gamma", PostProcessing::color.gamma);

	finalPassShader->setFloat("configuration.bloom", PostProcessing::bloom.enabled);
	finalPassShader->setFloat("configuration.bloomIntensity", PostProcessing::bloom.intensity);
	finalPassShader->setVec3("configuration.bloomColor", glm::vec3(PostProcessing::bloom.color[0], PostProcessing::bloom.color[1], PostProcessing::bloom.color[2]));
	finalPassShader->setFloat("configuration.bloomThreshold", PostProcessing::bloom.threshold);
	finalPassShader->setFloat("configuration.lensDirt", PostProcessing::bloom.lensDirtEnabled);
	finalPassShader->setFloat("configuration.lensDirtIntensity", PostProcessing::bloom.lensDirtIntensity);

	finalPassShader->setBool("configuration.chromaticAberration", PostProcessing::chromaticAberration.enabled);
	finalPassShader->setFloat("configuration.chromaticAberrationIntensity", PostProcessing::chromaticAberration.intensity);
	finalPassShader->setInt("configuration.chromaticAberrationIterations", PostProcessing::chromaticAberration.iterations);

	finalPassShader->setBool("configuration.vignette", PostProcessing::vignette.enabled);
	finalPassShader->setFloat("configuration.vignetteIntensity", PostProcessing::vignette.intensity);
	finalPassShader->setVec3("configuration.vignetteColor", glm::vec3(PostProcessing::vignette.color[0], PostProcessing::vignette.color[1], PostProcessing::vignette.color[2]));
	finalPassShader->setFloat("configuration.vignetteRadius", PostProcessing::vignette.radius);
	finalPassShader->setFloat("configuration.vignetteSoftness", PostProcessing::vignette.softness);
	finalPassShader->setFloat("configuration.vignetteRoundness", PostProcessing::vignette.roundness);
}