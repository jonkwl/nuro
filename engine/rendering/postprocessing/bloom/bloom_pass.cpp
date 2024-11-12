#include "bloom_pass.h"

#include "../engine/window/window.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/primitives/quad.h"
#include "../engine/rendering/postprocessing/bloom/bloom_frame.h"

glm::vec2 BloomPass::fViewportSize = glm::vec2(0.0f, 0.0f);
glm::ivec2 BloomPass::iViewportSize = glm::ivec2(0, 0);

float BloomPass::threshold = 0.0f;
float BloomPass::softThreshold = 0.0f;
float BloomPass::filterRadius = 0.0f;

Shader* BloomPass::prefilterShader = nullptr;
Shader* BloomPass::downsamplingShader = nullptr;
Shader* BloomPass::upsamplingShader = nullptr;

void BloomPass::setup()
{
	iViewportSize = glm::ivec2(Window::width, Window::height);
	fViewportSize = glm::vec2((float)Window::width, (float)Window::height);

	prefilterShader = ShaderBuilder::get("bloom_prefilter");
	downsamplingShader = ShaderBuilder::get("bloom_downsampling");
	upsamplingShader = ShaderBuilder::get("bloom_upsampling");

	const unsigned int mipDepth = 8;
	BloomFrame::setup(mipDepth);

	prefilterShader->bind();
	prefilterShader->setInt("inputTexture", 0);

	downsamplingShader->bind();
	downsamplingShader->setInt("inputTexture", 0);
	
	upsamplingShader->bind();
	upsamplingShader->setInt("inputTexture", 0);
}

unsigned int BloomPass::render(unsigned int input)
{
	BloomFrame::bind();

	unsigned int PREFILTERING_PASS_OUTPUT = prefilteringPass(input);
	downsamplingPass(PREFILTERING_PASS_OUTPUT);
	upsamplingPass();

	// Unbind framebuffer to restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, iViewportSize.x, iViewportSize.y);

	return BloomFrame::getMipChain()[0].texture;
}

unsigned int BloomPass::prefilteringPass(unsigned int input)
{
	unsigned int prefilterTarget = BloomFrame::getPrefilterTexture();

	prefilterShader->bind();
	prefilterShader->setFloat("threshold", threshold);
	prefilterShader->setFloat("softThreshold", softThreshold);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prefilterTarget, 0);

	Quad::bind();
	Quad::render();

	glBindTexture(GL_TEXTURE_2D, prefilterTarget);

	return prefilterTarget;
}

void BloomPass::downsamplingPass(unsigned int input)
{
	const std::vector<BloomMip>& mipChain = BloomFrame::getMipChain();

	downsamplingShader->bind();
	downsamplingShader->setVec2("resolution", fViewportSize);

	// Bind input as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, input);

	// Downsample through mip chain
	for (int i = 0; i < mipChain.size(); i++) {
		const BloomMip& mip = mipChain[i];
		
		glViewport(0, 0, mip.fSize.x, mip.fSize.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

		Quad::bind();
		Quad::render();

		downsamplingShader->setVec2("resolution", mip.fSize);
		
		glBindTexture(GL_TEXTURE_2D, mip.texture);
	}
}

void BloomPass::upsamplingPass()
{
	const std::vector<BloomMip>& mipChain = BloomFrame::getMipChain();

	upsamplingShader->bind();
	upsamplingShader->setFloat("filterRadius", filterRadius);

	// Set additive blending to enabled
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	// Upsample through mip chain
	for (int i = mipChain.size() - 1; i > 0; i--)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& forwardMip = mipChain[i - 1];

		// Set input texture for next render to be current mips texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set viewport and set render target
		glViewport(0, 0, forwardMip.fSize.x, forwardMip.fSize.y);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, forwardMip.texture, 0);

		Quad::bind();
		Quad::render();
	}

	// Disable additive blending
	glDisable(GL_BLEND);
}
