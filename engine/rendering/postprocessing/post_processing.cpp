#include "post_processing.h"

PostProcessingConfiguration PostProcessing::configuration = PostProcessingConfiguration();

Shader* PostProcessing::finalPassShader = nullptr;

void PostProcessing::setup()
{
	// Get default post processing finalPassShader
	finalPassShader = ShaderBuilder::get("final_pass");
}

void PostProcessing::render(unsigned int input)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input);

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	syncConfiguration();

	// Bind vao and texture and render framebuffer to screen
	Quad::bind();

	glDisable(GL_DEPTH_TEST);
	Quad::draw();
	glEnable(GL_DEPTH_TEST);
}

void PostProcessing::syncConfiguration()
{
	finalPassShader->setInt("inputTexture", 0);
	finalPassShader->setVec2("resolution", Window::getSize());

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
