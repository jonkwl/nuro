#include "bloom_frame.h"

#include "../src/window/window.h"
#include "../src/utils/log.h"

unsigned int BloomFrame::framebuffer = 0;
unsigned int BloomFrame::prefilterTexture = 0;
std::vector<BloomMip> BloomFrame::mipChain = std::vector<BloomMip>();

void BloomFrame::setup(unsigned int mipDepth)
{
	// Get initial viewport size
	glm::ivec2 iMipSize = glm::ivec2((int)Window::width, (int)Window::height);
	glm::vec2 fMipSize = glm::vec2((float)Window::width, (float)Window::height);

	// Generate framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate prefilter texture
	glGenTextures(1, &prefilterTexture);
	glBindTexture(GL_TEXTURE_2D, prefilterTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set prefilter texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Generate all bloom mips
	for (unsigned int i = 0; i < mipDepth; i++) {
		BloomMip mip;

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
		GL_COLOR_ATTACHMENT0
	};
	glDrawBuffers(1, fboAttachments);

	// Check for framebuffer errors
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating bloom framebuffer: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomFrame::bind()
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

unsigned int BloomFrame::getPrefilterTexture()
{
	return prefilterTexture;
}

std::vector<BloomMip>& BloomFrame::getMipChain()
{
	return mipChain;
}
