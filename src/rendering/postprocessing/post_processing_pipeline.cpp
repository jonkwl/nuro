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
#include "../src/runtime/runtime.h"
#include "../src/utils/log.h"

PostProcessingPipeline::PostProcessingPipeline(Viewport& viewport) : viewport(viewport),
fbo(0),
output(0),
finalPassShader(nullptr),
configuration(PostProcessing::configuration),
motionBlurPass(viewport),
bloomPass(viewport)
{
}

void PostProcessingPipeline::create()
{
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
	bloomPass.create(configuration.bloomMipDepth);
}

void PostProcessingPipeline::destroy()
{
	// Delete output texture
	glDeleteTextures(1, &output);
	output = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Remove shader
	finalPassShader = nullptr;
}

void PostProcessingPipeline::render(unsigned int hdrInput)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Pass input through post processing pipeline
	unsigned int POST_PROCESSING_PIPELINE_HDR = hdrInput;
	unsigned int POST_PROCESSING_PIPELINE_DEPTH = Runtime::prePassDepthOutput;
	unsigned int POST_PROCESSING_PIPELINE_NORMAL = Runtime::prePassNormalOutput;

	// Motion blur pass
	if (configuration.motionBlur)
	{
		// Apply motion blur on post processing hdr input
		POST_PROCESSING_PIPELINE_HDR = motionBlurPass.render(POST_PROCESSING_PIPELINE_HDR, POST_PROCESSING_PIPELINE_DEPTH);
	}

	// Seperate bloom pass
	unsigned int BLOOM_PASS_OUTPUT = 0;
	if (configuration.bloom)
	{
		bloomPass.threshold = configuration.bloomThreshold;
		bloomPass.softThreshold = configuration.bloomSoftThreshold;
		bloomPass.filterRadius = configuration.bloomFilterRadius;
		bloomPass.mipDepth = configuration.bloomMipDepth;
		BLOOM_PASS_OUTPUT = bloomPass.render(POST_PROCESSING_PIPELINE_HDR);
	}

	// glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind default framebuffer

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setVec2("resolution", glm ::vec2(viewport.width, viewport.height));

	// Sync post processing configuration with shader
	syncConfiguration();

	// Bind forward pass hdr color buffer
	glActiveTexture(GL_TEXTURE0 + HDR_UNIT);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_HDR);

	// Bind pre pass depth buffer
	glActiveTexture(GL_TEXTURE0 + DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_DEPTH);

	// Bind bloom buffer
	glActiveTexture(GL_TEXTURE0 + BLOOM_UNIT);
	glBindTexture(GL_TEXTURE_2D, BLOOM_PASS_OUTPUT);

	// Bind lens dirt texture
	if (configuration.lensDirt)
	{
		configuration.lensDirtTexture.bind(LENS_DIRT_UNIT);
	}

	// Bind quad and render to screen
	Quad::bind();
	Quad::render();

	// Unbind post processing framebuffer (rendering to screen at the moment)
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int PostProcessingPipeline::getOutput()
{
	return output;
}

void PostProcessingPipeline::syncConfiguration()
{
	finalPassShader->setFloat("configuration.exposure", configuration.exposure);
	finalPassShader->setFloat("configuration.contrast", configuration.contrast);
	finalPassShader->setFloat("configuration.gamma", configuration.gamma);

	finalPassShader->setFloat("configuration.bloom", configuration.bloom);
	finalPassShader->setFloat("configuration.bloomIntensity", configuration.bloomIntensity);
	finalPassShader->setVec3("configuration.bloomColor", glm::vec3(configuration.bloomColor[0], configuration.bloomColor[1], configuration.bloomColor[2]));
	finalPassShader->setFloat("configuration.bloomThreshold", configuration.bloomThreshold);
	finalPassShader->setFloat("configuration.lensDirt", configuration.lensDirt);
	finalPassShader->setFloat("configuration.lensDirtIntensity", configuration.lensDirtIntensity);

	finalPassShader->setBool("configuration.chromaticAberration", configuration.chromaticAberration);
	finalPassShader->setFloat("configuration.chromaticAberrationIntensity", configuration.chromaticAberrationIntensity);
	finalPassShader->setInt("configuration.chromaticAberrationIterations", configuration.chromaticAberrationIterations);

	finalPassShader->setBool("configuration.vignette", configuration.vignette);
	finalPassShader->setFloat("configuration.vignetteIntensity", configuration.vignetteIntensity);
	finalPassShader->setVec3("configuration.vignetteColor", glm::vec3(configuration.vignetteColor[0], configuration.vignetteColor[1], configuration.vignetteColor[2]));
	finalPassShader->setFloat("configuration.vignetteRadius", configuration.vignetteRadius);
	finalPassShader->setFloat("configuration.vignetteSoftness", configuration.vignetteSoftness);
	finalPassShader->setFloat("configuration.vignetteRoundness", configuration.vignetteRoundness);
}