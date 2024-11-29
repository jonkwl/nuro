#include "bloom_pass.h"

#include <glad/glad.h>

#include "../src/window/window.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/utils/log.h"

BloomPass::BloomPass() :
	threshold(0.0f),
	softThreshold(0.0f),
	filterRadius(0.0f),
	mipDepth(0),
	created(false),
	mipChain(),
	iViewportSize(0, 0),
	fViewportSize(0.0f, 0.0f),
	inversedViewportSize(0, 0),
	framebuffer(0),
	prefilterOutput(0),
	prefilterShader(nullptr),
	downsamplingShader(nullptr),
	upsamplingShader(nullptr)
{
}

void BloomPass::create(unsigned int mipDepth)
{
	if (created) return;

	// Load shaders
	prefilterShader = ShaderPool::get("bloom_prefilter");
	downsamplingShader = ShaderPool::get("bloom_downsampling");
	upsamplingShader = ShaderPool::get("bloom_upsampling");

	// Set static prefilter uniforms
	prefilterShader->bind();
	prefilterShader->setInt("inputTexture", 0);

	// Set static downsampling uniforms
	downsamplingShader->bind();
	downsamplingShader->setInt("inputTexture", 0);

	// Set static upsampling uniforms
	upsamplingShader->bind();
	upsamplingShader->setInt("inputTexture", 0);
	
	// Set mip depth member
	this->mipDepth = mipDepth;

	// Get initial viewport size
	iViewportSize = glm::ivec2(Window::width, Window::height);
	fViewportSize = glm::vec2((float)Window::width, (float)Window::height);
	inversedViewportSize = 1.0f / fViewportSize;

	// Get initial viewport size
	glm::ivec2 iMipSize = glm::ivec2((int)Window::width, (int)Window::height);
	glm::vec2 fMipSize = glm::vec2((float)Window::width, (float)Window::height);

	// Generate framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate prefilter texture
	glGenTextures(1, &prefilterOutput);
	glBindTexture(GL_TEXTURE_2D, prefilterOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set prefilter texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Generate all bloom mips
	for (unsigned int i = 0; i < mipDepth; i++)
	{
		BloomPass::Mip mip;

		// Halve mips size
		iMipSize /= 2;
		fMipSize *= 0.5f;

		mip.iSize = iMipSize;
		mip.fSize = fMipSize;
		mip.inversedSize = 1.0f / fMipSize;

		// Generate mips texture
		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, iMipSize.x, iMipSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);

		// Set mip textures parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Add generated mip to bloom mip chain
		mipChain.emplace_back(mip);
	}

	// Set framebuffer attachments, set first mip to be the color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].texture, 0);

	unsigned int fboAttachments[1] = {
		GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, fboAttachments);

	// Check for framebuffer errors
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Log::printError("Bloom Pass", "Error generating bloom framebuffer: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	created = true;
}

void BloomPass::destroy()
{
	if (!created) return;

	// Delete prefilter texture
	glDeleteTextures(1, &prefilterOutput);
	prefilterOutput = 0;

	// Delete all mipmap texture
	for (auto& mip : mipChain) {
		glDeleteTextures(1, &mip.texture);
	}

	// Clear mipchain
	mipChain.clear();

	// Delete framebuffer
	glDeleteFramebuffers(1, &framebuffer);
	framebuffer = 0;

	// Remove shaders
	prefilterShader = nullptr;
	downsamplingShader = nullptr;
	upsamplingShader = nullptr;

	created = false;
}

unsigned int BloomPass::render(unsigned int hdrInput)
{
	if (!created) return Log::printUncreatedWarning("Bloom Pass", "render");

	// Bind bloom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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
	return mipChain[0].texture;
}

unsigned int BloomPass::prefilteringPass(unsigned int hdrInput)
{
	// Set prefilter uniforms
	prefilterShader->bind();
	prefilterShader->setFloat("threshold", threshold);
	prefilterShader->setFloat("softThreshold", softThreshold);

	// Bind input texture for prefilter pass
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrInput);

	// Set prefilter target texture as framebuffer render target
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prefilterOutput, 0);

	// Bind and render to quad
	Quad::bind();
	Quad::render();

	// Return prefiltering pass target (now the output after rendering)
	return prefilterOutput;
}

void BloomPass::downsamplingPass(unsigned int hdrInput)
{
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
		const BloomPass::Mip &mip = mipChain[i];

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
		const BloomPass::Mip &mip = mipChain[i];
		const BloomPass::Mip &targetMip = mipChain[i - 1];

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