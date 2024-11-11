#include "post_processing.h"

PostProcessingConfiguration PostProcessing::configuration = PostProcessingConfiguration();

Shader* PostProcessing::finalPassShader = nullptr;

void PostProcessing::setup()
{
	// Get default post processing finalPassShader
	finalPassShader = ShaderBuilder::get("final_pass");

	// Setup post processing pipeline
	DebugPass::setup();
}

void PostProcessing::render(unsigned int input)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Pass input through post processing pipeline
	unsigned int POST_PROCESSING_PIPELINE_OUTPUT = DebugPass::render(input);

	// Bind default framebuffer to render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind input texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_OUTPUT);

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setInt("inputTexture", 0);
	finalPassShader->setVec2("resolution", Window::getSize());

	// Sync post processing configuration with shader
	syncConfiguration();

	// Bind quad and render to screen
	Quad::bind();
	Quad::render();
}

void PostProcessing::syncConfiguration()
{
	finalPassShader->setFloat("configuration.exposure", configuration.exposure);
	finalPassShader->setFloat("configuration.contrast", configuration.contrast);
	finalPassShader->setFloat("configuration.gamma", configuration.gamma);

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
