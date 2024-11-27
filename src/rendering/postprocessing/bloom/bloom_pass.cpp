#include "bloom_pass.h"

#include <glad/glad.h>

#include "../src/window/window.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/rendering/postprocessing/bloom/bloom_frame.h"

glm::ivec2 BloomPass::iViewportSize = glm::ivec2(0, 0);
glm::vec2 BloomPass::fViewportSize = glm::vec2(0.0f, 0.0f);
glm::vec2 BloomPass::inversedViewportSize = glm::ivec2(0, 0);

float BloomPass::threshold = 0.0f;
float BloomPass::softThreshold = 0.0f;
float BloomPass::filterRadius = 0.0f;
unsigned int BloomPass::mipDepth = 16;

Shader *BloomPass::prefilterShader = nullptr;
Shader *BloomPass::downsamplingShader = nullptr;
Shader *BloomPass::upsamplingShader = nullptr;

void BloomPass::setup()
{
	// Get initial viewport size
	iViewportSize = glm::ivec2(Window::width, Window::height);
	fViewportSize = glm::vec2((float)Window::width, (float)Window::height);
	inversedViewportSize = 1.0f / fViewportSize;

	// Get shaders
	prefilterShader = ShaderPool::get("bloom_prefilter");
	downsamplingShader = ShaderPool::get("bloom_downsampling");
	upsamplingShader = ShaderPool::get("bloom_upsampling");

	// Setup bloom frame
	BloomFrame::setup(mipDepth);

	// Set static prefilter uniforms
	prefilterShader->bind();
	prefilterShader->setInt("inputTexture", 0);

	// Set static downsampling uniforms
	downsamplingShader->bind();
	downsamplingShader->setInt("inputTexture", 0);

	// Set static upsampling uniforms
	upsamplingShader->bind();
	upsamplingShader->setInt("inputTexture", 0);
}

unsigned int BloomPass::render(unsigned int hdrInput)
{
	// Bind bloom framebuffer
	BloomFrame::bind();

	// Perform prefiltering pass
	unsigned int PREFILTERING_PASS_OUTPUT = prefilteringPass(hdrInput);

	// Perform downsampling pass
	downsamplingPass(PREFILTERING_PASS_OUTPUT);

	// Perform upsampling pass
	upsamplingPass();

	// Unbind framebuffer to restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, iViewportSize.x, iViewportSize.y);

	// Return texture of first bloom mip (the texture being rendered to)
	return BloomFrame::getMipChain()[0].texture;
}

unsigned int BloomPass::prefilteringPass(unsigned int hdrInput)
{
	// Get target texture for prefiltering pass
	unsigned int prefilterTarget = BloomFrame::getPrefilterTexture();

	// Set prefilter uniforms
	prefilterShader->bind();
	prefilterShader->setFloat("threshold", threshold);
	prefilterShader->setFloat("softThreshold", softThreshold);

	// Bind input texture for prefilter pass
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	// Set prefilter target texture as framebuffer render target
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prefilterTarget, 0);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Return prefiltering pass target (now the output after rendering)
	return prefilterTarget;
}

void BloomPass::downsamplingPass(unsigned int hdrInput)
{
	// Get bloom mip chain
	const std::vector<BloomMip> &mipChain = BloomFrame::getMipChain();

	// Set downsampling uniforms
	downsamplingShader->bind();
	downsamplingShader->setVec2("inversedResolution", inversedViewportSize);

	// Bind input as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	// Downsample through mip chain
	for (int i = 0; i < mipChain.size(); i++)
	{
		// Get current mip
		const BloomMip &mip = mipChain[i];

		// Set viewport and framebuffer rendering target according to current mip
		glViewport(0, 0, mip.fSize.x, mip.fSize.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

		// Bind and render to quad
		Quad::bind();
		Quad::render();

		// Set inversed resolution for next downsample iteration
		downsamplingShader->setVec2("inversedResolution", mip.inversedSize);

		// Bind mip texture for next downsample iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
	}
}

void BloomPass::upsamplingPass()
{
	// Get bloom mip chain
	const std::vector<BloomMip> &mipChain = BloomFrame::getMipChain();

	// Get viewport aspect ratio
	const float aspectRatio = fViewportSize.x / fViewportSize.y;

	// Set downsampling uniforms
	upsamplingShader->bind();
	upsamplingShader->setFloat("filterRadius", filterRadius);
	upsamplingShader->setFloat("aspectRatio", aspectRatio);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	// Upsample through mip chain
	for (int i = mipChain.size() - 1; i > 0; i--)
	{
		// Get current mip and target mip for current downsampling iteration
		const BloomMip &mip = mipChain[i];
		const BloomMip &targetMip = mipChain[i - 1];

		// Set input texture for next render to be current mips texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set viewport and set render target
		glViewport(0, 0, targetMip.fSize.x, targetMip.fSize.y);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetMip.texture, 0);

		// Bind and render to quad
		Quad::bind();
		Quad::render();
	}

	// Disable additive blending
	glDisable(GL_BLEND);
}
