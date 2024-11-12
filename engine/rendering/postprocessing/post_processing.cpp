#include "post_processing.h"

PostProcessingConfiguration PostProcessing::configuration = PostProcessingConfiguration();

Shader* PostProcessing::finalPassShader = nullptr;

void PostProcessing::setup()
{
	// Get default post processing final pass shader
	finalPassShader = ShaderBuilder::get("final_pass");

	// Bind final pass shader and set static uniforms
	finalPassShader->bind();
	finalPassShader->setInt("hdrBuffer", HDR_BUFFER_UNIT);
	finalPassShader->setInt("bloomBuffer", BLOOM_BUFFER_UNIT);

	// Setup post processing pipeline
	BloomPass::setup();
	DebugPass::setup();
}

void PostProcessing::render(unsigned int input)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Seperate bloom pass
	BloomPass::threshold = configuration.bloomThreshold;
	BloomPass::softThreshold = configuration.bloomSoftThreshold;
	BloomPass::filterRadius = configuration.bloomFilterRadius;
	unsigned int BLOOM_PASS_OUTPUT = BloomPass::render(input);

	// Pass input through post processing pipeline
	unsigned int POST_PROCESSING_PIPELINE_OUTPUT = input;

	// Bind default framebuffer to render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setVec2("resolution", Window::getSize());

	// Sync post processing configuration with shader
	syncConfiguration();

	// Bind textures for final post processing pass render
	glActiveTexture(GL_TEXTURE0 + HDR_BUFFER_UNIT);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_OUTPUT);

	glActiveTexture(GL_TEXTURE0 + BLOOM_BUFFER_UNIT);
	glBindTexture(GL_TEXTURE_2D, BLOOM_PASS_OUTPUT);

	// Bind quad and render to screen
	Quad::bind();
	Quad::render();
}

void PostProcessing::syncConfiguration()
{
	finalPassShader->setFloat("configuration.exposure", configuration.exposure);
	finalPassShader->setFloat("configuration.contrast", configuration.contrast);
	finalPassShader->setFloat("configuration.gamma", configuration.gamma);

	finalPassShader->setFloat("configuration.bloomIntensity", configuration.bloomIntensity);
	finalPassShader->setVec3("configuration.bloomColor", configuration.bloomColor);
	finalPassShader->setFloat("configuration.bloomBlend", configuration.bloomBlend);
	finalPassShader->setFloat("configuration.bloomThreshold", configuration.bloomThreshold);

	finalPassShader->setBool("configuration.chromaticAberration", configuration.chromaticAberration);
	finalPassShader->setFloat("configuration.chromaticAberrationStrength", configuration.chromaticAberrationStrength);
	finalPassShader->setFloat("configuration.chromaticAberrationRange", configuration.chromaticAberrationRange);
	finalPassShader->setFloat("configuration.chromaticAberrationRedOffset", configuration.chromaticAberrationRedOffset);
	finalPassShader->setFloat("configuration.chromaticAberrationBlueOffset", configuration.chromaticAberrationBlueOffset);

	finalPassShader->setBool("configuration.vignette", configuration.vignette);
	finalPassShader->setFloat("configuration.vignetteStrength", configuration.vignetteStrength);
	finalPassShader->setVec3("configuration.vignetteColor", configuration.vignetteColor);
	finalPassShader->setFloat("configuration.vignetteRadius", configuration.vignetteRadius);
	finalPassShader->setFloat("configuration.vignetteSoftness", configuration.vignetteSoftness);
	finalPassShader->setFloat("configuration.vignetteRoundness", configuration.vignetteRoundness);
}
