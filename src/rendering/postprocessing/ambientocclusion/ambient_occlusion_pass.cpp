#include "ambient_occlusion_pass.h"

#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"

unsigned int AmbientOcclusionPass::fbo = 0;

unsigned int AmbientOcclusionPass::aoOutput = 0;
unsigned int AmbientOcclusionPass::compositeOutput = 0;

Shader* AmbientOcclusionPass::aoPassShader = nullptr;
Shader* AmbientOcclusionPass::compositeShader = nullptr;

std::vector<glm::vec3> AmbientOcclusionPass::kernel;
unsigned int AmbientOcclusionPass::noiseTexture = 0;

void AmbientOcclusionPass::setup()
{
	// Default kernel setup
	unsigned int nSamples = 64;
	float noiseSize = 4.0f;

	// Get sample kernel and noise textures
	kernel = generateKernel(nSamples);
	noiseTexture = generateNoiseTexture(nSamples, noiseSize);

	// Set ambient occlusion pass shaders static uniforms
	aoPassShader = ShaderPool::get("ambient_occlusion_pass");
	aoPassShader->bind();
	aoPassShader->setInt("depthInput", AMBIENT_OCCLUSION_DEPTH_UNIT);
	aoPassShader->setInt("normalInput", AMBIENT_OCCLUSION_NORMAL_UNIT);
	aoPassShader->setInt("noiseTexture", AMBIENT_OCCLUSION_NOISE_UNIT);
	aoPassShader->setFloat("noiseSize", noiseSize);
	for (int i = 0; i < nSamples; ++i) {
		aoPassShader->setVec3("samples[" + std::to_string(i) + "]", kernel[i]);
	}

	// Set ambient occlusion composite shaders static uniforms
	compositeShader = ShaderPool::get("ambient_occlusion_composite");
	compositeShader->bind();
	compositeShader->setInt("hdrInput", AMBIENT_OCCLUSION_HDR_UNIT);
	compositeShader->setInt("aoInput", AMBIENT_OCCLUSION_AO_UNIT);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate ambient occlusion output texture
	glGenTextures(1, &aoOutput);
	glBindTexture(GL_TEXTURE_2D, aoOutput);
	// IMPROVEMENT: Reduce to one channel R, as only one channel is needed for ambient occlusion value
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set ambient occlusion output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach ambient occlusion output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoOutput, 0);

	// Generate composite output texture
	glGenTextures(1, &compositeOutput);
	glBindTexture(GL_TEXTURE_2D, compositeOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set composite output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating fbo: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int AmbientOcclusionPass::render(unsigned int hdrInput, unsigned int depthInput, unsigned int normalInput)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Perform ambient occlusion pass
	ambientOcclusionPass(depthInput, normalInput);

	// Perform composite pass: Apply ambient occlusion from previous pass to hdr input
	compositePass(hdrInput);

	// Return output
	return aoOutput;
}

void AmbientOcclusionPass::ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput)
{
	// Set render target to ao output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoOutput, 0);

	// Bind ambient occlusion pass shader
	aoPassShader->bind();

	// Set ambient occlusion pass shader uniforms
	aoPassShader->setVec2("resolution", glm::vec2(Window::width, Window::height));
	aoPassShader->setMatrix4("projectionMatrix", MeshRenderer::currentProjectionMatrix);
	aoPassShader->setMatrix4("inverseProjectionMatrix", glm::inverse(MeshRenderer::currentProjectionMatrix));

	// Bind depth input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Bind normal input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_NORMAL_UNIT);
	glBindTexture(GL_TEXTURE_2D, normalInput);

	// Bind noise texture
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_NOISE_UNIT);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	// Bind and render to quad
	Quad::bind();
	Quad::render();
}

// TEMPORARY DEBUGGING PASS!
void AmbientOcclusionPass::compositePass(unsigned int hdrInput)
{
	// Set render target to composite output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, compositeOutput, 0);

	// Bind composite shader
	compositeShader->bind();

	// Bind hdr input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_HDR_UNIT);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	// Bind ao input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_AO_UNIT);
	glBindTexture(GL_TEXTURE_2D, aoOutput);

	// Bind and render to quad
	Quad::bind();
	Quad::render();
}

std::vector<glm::vec3> AmbientOcclusionPass::generateKernel(unsigned int nSamples)
{
	std::vector<glm::vec3> kernel;

	for (int i = 0; i < nSamples; ++i) {
		// Generate raw sample with random values
		glm::vec3 sample = glm::vec3(random() * 2.0f - 1.0f, random() * 2.0f - 1.0f, random());

		// Normalize sample
		sample = glm::normalize(sample);

		// Weight with random value
		sample *= random();

		// More weight to samples closer to the center
		float scale = (float)i / (float)nSamples;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		// Add sample to kernel
		kernel.push_back(sample);
	}

	return kernel;
}

unsigned int AmbientOcclusionPass::generateNoiseTexture(unsigned int nSamples, float size)
{
	// Generate noise samples
	std::vector<glm::vec3> noiseSamples;

	for (int i = 0; i < nSamples; i++) {
		glm::vec3 sample = glm::vec3(random() * 2.0f - 1.0f, random() * 2.0f - 1.0f, 0.0f);
		noiseSamples.push_back(sample);
	}

	// Generate noise texture with noise samples
	unsigned int output = 0;
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size, size, 0, GL_RGB, GL_FLOAT, &noiseSamples[0]);

	// Set noise texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Return noise texture
	return output;
}

float AmbientOcclusionPass::random()
{
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	std::default_random_engine generator;
	return distribution(generator);
}

float AmbientOcclusionPass::lerp(float start, float end, float value)
{
	return start + value * (end - start);
}
