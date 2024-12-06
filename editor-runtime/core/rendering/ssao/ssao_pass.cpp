#include "ssao_pass.h"

#include <glad/glad.h>
#include <random>

#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/core/mesh_renderer.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/utils/log.h"

SSAOPass::SSAOPass(Viewport& viewport) : viewport(viewport),
aoScale(0.0f),
maxKernelSamples(0),
noiseResolution(0.0f),
fbo(0),
aoOutput(0),
blurredOutput(0),
aoPassShader(nullptr),
aoBlurShader(nullptr),
kernel(),
noiseTexture(0)
{
}

void SSAOPass::create(float aoScale, int maxKernelSamples, float noiseResolution)
{
	// Set members
	this->aoScale = aoScale;
	this->maxKernelSamples = maxKernelSamples;
	this->noiseResolution = noiseResolution;

	// Get sample kernel and noise textures
	kernel = generateKernel();
	noiseTexture = generateNoiseTexture();

	// Set ambient occlusion pass shaders static uniforms
	aoPassShader = ShaderPool::get("ssao_pass");
	aoPassShader->bind();
	aoPassShader->setInt("depthInput", DEPTH_UNIT);
	aoPassShader->setInt("normalInput", NORMAL_UNIT);
	aoPassShader->setInt("noiseTexture", NOISE_UNIT);
	aoPassShader->setFloat("noiseSize", noiseResolution);
	for (int i = 0; i < maxKernelSamples; ++i)
	{
		aoPassShader->setVec3("samples[" + std::to_string(i) + "]", kernel[i]);
	}

	// Set ambient occlusion blur shaders static uniforms
	aoBlurShader = ShaderPool::get("ssao_blur");
	aoBlurShader->bind();
	aoBlurShader->setInt("ssaoInput", AO_UNIT);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate ambient occlusion output texture
	glGenTextures(1, &aoOutput);
	glBindTexture(GL_TEXTURE_2D, aoOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, viewport.width * aoScale, viewport.height * aoScale, 0, GL_RED, GL_FLOAT, nullptr);

	// Set ambient occlusion output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach ambient occlusion output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoOutput, 0);

	// Generate blurred ambient occlusion output texture
	glGenTextures(1, &blurredOutput);
	glBindTexture(GL_TEXTURE_2D, blurredOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, viewport.width, viewport.height, 0, GL_RED, GL_FLOAT, nullptr);

	// Set blurred ambient occlusion output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Framebuffer", "Error generating fbo: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOPass::destroy() {
	// Reset configuration
	aoScale = 0;
	maxKernelSamples = 0.0f;
	noiseResolution = 0;

	// Delete ambient occlusion output texture
	glDeleteTextures(1, &aoOutput);
	aoOutput = 0;

	// Delete blurred output texture
	glDeleteTextures(1, &blurredOutput);
	blurredOutput = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Reset shaders
	aoPassShader = nullptr;
	aoBlurShader = nullptr;

	// Clear kernel
	kernel.clear();

	// Delete noise texture
	glDeleteTextures(1, &noiseTexture);
	noiseTexture = 0;
}

unsigned int SSAOPass::render(const PostProcessing::Profile& profile, unsigned int depthInput, unsigned int normalInput)
{
	// Disable depth testing and culling
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Perform ambient occlusion pass
	ambientOcclusionPass(profile, depthInput, normalInput);

	// Perform blur pass: Blur ambient occlusion
	// UNKNOWN ISSUE WITH BLUR PASS
	blurPass(profile);

	// Re-Enable depth testing and culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Return blurred output
	// return blurredOutput;
	return aoOutput; // tmp return raw ao output
}

unsigned int SSAOPass::getOutputRaw()
{
	return aoOutput;
}

unsigned int SSAOPass::getOutputProcessed()
{
	return blurredOutput;
}

void SSAOPass::ambientOcclusionPass(const PostProcessing::Profile& profile, unsigned int depthInput, unsigned int normalInput)
{
	// Set render target to ao output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoOutput, 0);

	// Set viewport size
	glViewport(0, 0, viewport.width * aoScale, viewport.height * aoScale);

	// Get current sample amount, make sure its not higher than the maximum sample amount
	int nSamples = profile.ambientOcclusion.samples;
	if (nSamples > maxKernelSamples)
	{
		nSamples = maxKernelSamples;
	}

	// Bind ambient occlusion pass shader
	aoPassShader->bind();

	// Set ambient occlusion pass shader uniforms
	aoPassShader->setVec2("resolution", glm::vec2(viewport.width, viewport.height));
	aoPassShader->setMatrix4("projectionMatrix", MeshRenderer::currentProjectionMatrix);
	aoPassShader->setMatrix4("inverseProjectionMatrix", glm::inverse(MeshRenderer::currentProjectionMatrix));

	aoPassShader->setInt("nSamples", nSamples);
	aoPassShader->setFloat("radius", profile.ambientOcclusion.radius);
	aoPassShader->setFloat("bias", profile.ambientOcclusion.bias);
	aoPassShader->setFloat("power", profile.ambientOcclusion.power);

	// Bind depth input
	glActiveTexture(GL_TEXTURE0 + DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Bind normal input
	glActiveTexture(GL_TEXTURE0 + NORMAL_UNIT);
	glBindTexture(GL_TEXTURE_2D, normalInput);

	// Bind noise texture
	glActiveTexture(GL_TEXTURE0 + NOISE_UNIT);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	// Bind and render to quad
	Quad::bind();
	Quad::render();
}

void SSAOPass::blurPass(const PostProcessing::Profile& profile)
{
	// Set render target to blurred ambient occlusion output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurredOutput, 0);

	// Set viewport size
	glViewport(0, 0, viewport.width, viewport.height);

	// Bind blur shader
	aoBlurShader->bind();

	// Bind ao input
	glActiveTexture(GL_TEXTURE0 + AO_UNIT);
	glBindTexture(GL_TEXTURE_2D, aoOutput);

	// Bind and render to quad
	Quad::bind();
	Quad::render();
}

std::vector<glm::vec3> SSAOPass::generateKernel()
{
	std::vector<glm::vec3> kernel;

	for (int i = 0; i < maxKernelSamples; ++i)
	{
		// Generate raw sample with random values
		glm::vec3 sample = glm::vec3(random() * 2.0f - 1.0f, random() * 2.0f - 1.0f, random());

		// Normalize sample
		sample = glm::normalize(sample);

		// Weight with random value
		sample *= random();

		// More weight to samples closer to the center
		float scale = (float)i / (float)maxKernelSamples;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		// Add sample to kernel
		kernel.push_back(sample);
	}

	return kernel;
}

unsigned int SSAOPass::generateNoiseTexture()
{
	// Generate noise samples
	std::vector<glm::vec3> noiseSamples;

	for (int i = 0; i < maxKernelSamples; i++)
	{
		glm::vec3 sample = glm::vec3(random() * 2.0f - 1.0f, random() * 2.0f - 1.0f, 0.0f);
		noiseSamples.push_back(sample);
	}

	// Generate noise texture with noise samples
	unsigned int output = 0;
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, noiseResolution, noiseResolution, 0, GL_RGB, GL_FLOAT, &noiseSamples[0]);

	// Set noise texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Return noise texture
	return output;
}

float SSAOPass::random()
{
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	std::default_random_engine generator;
	return distribution(generator);
}

float SSAOPass::lerp(float start, float end, float value)
{
	return start + value * (end - start);
}