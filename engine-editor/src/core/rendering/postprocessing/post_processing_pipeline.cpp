#include "post_processing_pipeline.h"

#include <glad/glad.h>
#include <glm.hpp>

#include "../src/core/utils/console.h"
#include "../src/core/rendering/shader/shader.h"
#include "../src/core/rendering/shader/shader_pool.h"
#include "../src/core/rendering/primitives/global_quad.h"
#include "../src/core/rendering/passes/ssao_pass.h"
#include "../src/core/rendering/passes/pre_pass.h"
#include "../src/core/rendering/passes/forward_pass.h"
#include "../src/core/rendering/texture/texture.h"
#include "../src/core/utils/console.h"

PostProcessingPipeline::PostProcessingPipeline(const Viewport& viewport, const bool renderToScreen) : viewport(viewport),
renderToScreen(renderToScreen),
fbo(0),
output(0),
finalPassShader(ShaderPool::empty()),
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.getWidth_gl(), viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);

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
			Console::out::warning("Post Processing Pipeline", "Issue while generating framebuffer: " + std::to_string(fboStatus));
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
	bloomPass.create(DEFAULT_BLOOM_MIP_DEPTH);
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

void PostProcessingPipeline::render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const PostProcessing::Profile& profile, const uint32_t hdrInput, const uint32_t depthInput, const uint32_t velocityBufferInput)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Pass input through post processing pipeline
	uint32_t POST_PROCESSING_PIPELINE_HDR = hdrInput;

	// Motion blur pass
	if (profile.motionBlur.enabled)
	{
		// Apply motion blur on post processing hdr input
		POST_PROCESSING_PIPELINE_HDR = motionBlurPass.render(view, projection, viewProjection, profile, POST_PROCESSING_PIPELINE_HDR, depthInput, velocityBufferInput);
	}

	// Seperate bloom pass
	uint32_t BLOOM_PASS_OUTPUT = 0;
	if (profile.bloom.enabled)
	{
		bloomPass.threshold = profile.bloom.threshold;
		bloomPass.softThreshold = profile.bloom.softThreshold;
		bloomPass.filterRadius = profile.bloom.filterRadius;
		BLOOM_PASS_OUTPUT = bloomPass.render(POST_PROCESSING_PIPELINE_HDR);
	}

	// Bind post processing framebuffer (which is 0 if rendering to screen)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setVec2("resolution", viewport.getResolution());

	// Sync post processing configuration with shader
	syncConfiguration(profile);

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
	if (profile.bloom.lensDirtEnabled)
	{
		glActiveTexture(GL_TEXTURE0 + LENS_DIRT_UNIT);
		glBindTexture(GL_TEXTURE_2D, profile.bloom.lensDirtTexture);
	}

	// Bind quad and render to screen
	GlobalQuad::bind();
	GlobalQuad::render();

	// Unbind post processing framebuffer (redundant if rendering to screen)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t PostProcessingPipeline::getOutput()
{
	// Return output of post processing pipeline pass
	return output;
}

void PostProcessingPipeline::syncConfiguration(const PostProcessing::Profile& profile)
{
	finalPassShader->setFloat("configuration.exposure", profile.color.exposure);
	finalPassShader->setFloat("configuration.contrast", profile.color.contrast);
	finalPassShader->setFloat("configuration.gamma", profile.color.gamma);

	finalPassShader->setFloat("configuration.bloom", profile.bloom.enabled);
	finalPassShader->setFloat("configuration.bloomIntensity", profile.bloom.intensity);
	finalPassShader->setVec3("configuration.bloomColor", profile.bloom.color);
	finalPassShader->setFloat("configuration.bloomThreshold", profile.bloom.threshold);
	finalPassShader->setFloat("configuration.lensDirt", profile.bloom.lensDirtEnabled);
	finalPassShader->setFloat("configuration.lensDirtIntensity", profile.bloom.lensDirtIntensity);

	finalPassShader->setBool("configuration.chromaticAberration", profile.chromaticAberration.enabled);
	finalPassShader->setFloat("configuration.chromaticAberrationIntensity", profile.chromaticAberration.intensity);
	finalPassShader->setInt("configuration.chromaticAberrationIterations", profile.chromaticAberration.iterations);

	finalPassShader->setBool("configuration.vignette", profile.vignette.enabled);
	finalPassShader->setFloat("configuration.vignetteIntensity", profile.vignette.intensity);
	finalPassShader->setVec3("configuration.vignetteColor", profile.vignette.color);
	finalPassShader->setFloat("configuration.vignetteRadius", profile.vignette.radius);
	finalPassShader->setFloat("configuration.vignetteSoftness", profile.vignette.softness);
	finalPassShader->setFloat("configuration.vignetteRoundness", profile.vignette.roundness);
}