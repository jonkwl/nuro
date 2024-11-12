#include "bloom_frame.h"

#include "../engine/window/window.h"
#include "../engine/utils/log.h"

unsigned int BloomFrame::fbo = 0;
unsigned int BloomFrame::prefilterTexture = 0;
std::vector<BloomMip> BloomFrame::mipChain = std::vector<BloomMip>();

void BloomFrame::setup(unsigned int mipDepth)
{
	glm::ivec2 iMipSize = glm::ivec2((int)Window::width, (int)Window::height);
	glm::vec2 fMipSize = glm::vec2((float)Window::width, (float)Window::height);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &prefilterTexture);
	glBindTexture(GL_TEXTURE_2D, prefilterTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < mipDepth; i++) {
		BloomMip mip;

		iMipSize /= 2;
		fMipSize *= 0.5f;

		mip.iSize = iMipSize;
		mip.fSize = fMipSize;
		mip.inversedSize = 1.0f / fMipSize;

		glGenTextures(1, &mip.texture);
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, iMipSize.x, iMipSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		mipChain.emplace_back(mip);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mipChain[0].texture, 0);

	unsigned int fboAttachments[1] = {
		GL_COLOR_ATTACHMENT0
	};
	glDrawBuffers(1, fboAttachments);

	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}
}

void BloomFrame::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

unsigned int BloomFrame::getPrefilterTexture()
{
	return prefilterTexture;
}

std::vector<BloomMip>& BloomFrame::getMipChain()
{
	return mipChain;
}
