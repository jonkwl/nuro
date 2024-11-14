#include "post_processing.h"

PostProcessingConfiguration PostProcessing::configuration = PostProcessingConfiguration();

Shader* PostProcessing::finalPassShader = nullptr;

PostProcessingConfiguration PostProcessing::defaultConfiguration = PostProcessingConfiguration();

unsigned int PostProcessing::fbo = 0;
unsigned int PostProcessing::output = 0;

void PostProcessing::setup()
{
	// Create post processing framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating post processing framebuffer: " + std::to_string(fboStatus));
	}

	// Get default post processing final pass shader
	finalPassShader = ShaderBuilder::get("final_pass");

	// Bind final pass shader and set static uniforms
	finalPassShader->bind();
	finalPassShader->setInt("hdrBuffer", HDR_BUFFER_UNIT);
	finalPassShader->setInt("bloomBuffer", BLOOM_BUFFER_UNIT);
	finalPassShader->setInt("configuration.lensDirtTexture", LENS_DIRT_UNIT);

	// Cache default post processing configuration
	defaultConfiguration = configuration;

	// Setup post processing pipeline
	BloomPass::setup();
	DebugPass::setup();
}

void PostProcessing::render(unsigned int input)
{
	// Disable any depth testing for whole post processing pass
	glDisable(GL_DEPTH_TEST);

	// Seperate bloom pass
	unsigned int BLOOM_PASS_OUTPUT = 0;
	if (configuration.bloom) {
		BloomPass::threshold = configuration.bloomThreshold;
		BloomPass::softThreshold = configuration.bloomSoftThreshold;
		BloomPass::filterRadius = configuration.bloomFilterRadius;
		BloomPass::mipDepth = configuration.bloomMipDepth;
		BLOOM_PASS_OUTPUT = BloomPass::render(input);
	}

	// Pass input through post processing pipeline
	unsigned int POST_PROCESSING_PIPELINE_OUTPUT = input;

	// glBindFramebuffer(GL_FRAMEBUFFER, fbo); // Bind post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind default framebuffer

	// Bind finalPassShader and set uniforms
	finalPassShader->bind();
	finalPassShader->setVec2("resolution", Window::getSize());

	// check for default configurations
	if (!configuration.colorGrading) {
		configuration.contrast = defaultConfiguration.contrast;
		configuration.exposure = defaultConfiguration.exposure;
		configuration.gamma = defaultConfiguration.gamma;
	}

	// Sync post processing configuration with shader
	syncConfiguration();

	// Bind textures for final post processing pass render
	glActiveTexture(GL_TEXTURE0 + HDR_BUFFER_UNIT);
	glBindTexture(GL_TEXTURE_2D, POST_PROCESSING_PIPELINE_OUTPUT);

	glActiveTexture(GL_TEXTURE0 + BLOOM_BUFFER_UNIT);
	glBindTexture(GL_TEXTURE_2D, BLOOM_PASS_OUTPUT);

	if (configuration.lensDirt) {
		configuration.lensDirtTexture->bind(LENS_DIRT_UNIT);
	}

	// Bind quad and render to screen
	Quad::bind();
	Quad::render();

	// Unbind post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int PostProcessing::getOutput()
{
	return output;
}

void PostProcessing::syncConfiguration()
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
	finalPassShader->setFloat("configuration.chromaticAberrationRange", configuration.chromaticAberrationRange);
	finalPassShader->setFloat("configuration.chromaticAberrationRedOffset", configuration.chromaticAberrationRedOffset);
	finalPassShader->setFloat("configuration.chromaticAberrationBlueOffset", configuration.chromaticAberrationBlueOffset);

	finalPassShader->setBool("configuration.vignette", configuration.vignette);
	finalPassShader->setFloat("configuration.vignetteIntensity", configuration.vignetteIntensity);
	finalPassShader->setVec3("configuration.vignetteColor", glm::vec3(configuration.vignetteColor[0], configuration.vignetteColor[1], configuration.vignetteColor[2]));
	finalPassShader->setFloat("configuration.vignetteRadius", configuration.vignetteRadius);
	finalPassShader->setFloat("configuration.vignetteSoftness", configuration.vignetteSoftness);
	finalPassShader->setFloat("configuration.vignetteRoundness", configuration.vignetteRoundness);
}
