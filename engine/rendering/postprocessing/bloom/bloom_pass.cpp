#include "bloom_pass.h"

#include "../engine/window/window.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/primitives/quad.h"

void BloomPass::setup()
{
	iViewportSize = glm::ivec2(Window::width, Window::height);
	fViewportSize = glm::vec2((float)Window::width, (float)Window::height);

	downsamplingShader = ShaderBuilder::get("bloom_downsampling");
	upsamplingShader = ShaderBuilder::get("bloom_upsampling");

	const unsigned int mipDepth = 8;
	bloomFrame.setup(mipDepth);

	downsamplingShader->bind();
	downsamplingShader->setInt("inputTexture", 0);
	
	upsamplingShader->bind();
	upsamplingShader->setInt("inputTexture", 0);
}

unsigned int BloomPass::render(unsigned int input, float filterRadius)
{
	bloomFrame.bind();

	downsamplingPass(input);
	upsamplingPass(filterRadius);

	// Unbind framebuffer to restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, iViewportSize.x, iViewportSize.y);

	return bloomFrame.getMipChain()[0].texture;
}

void BloomPass::downsamplingPass(unsigned int input)
{
	const std::vector<BloomMip>& mipChain = bloomFrame.getMipChain();

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

void BloomPass::upsamplingPass(float filterRadius)
{
	const std::vector<BloomMip>& mipChain = bloomFrame.getMipChain();

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
