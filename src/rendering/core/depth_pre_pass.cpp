#include "depth_pre_pass.h"

#include "../src/runtime/runtime.h"

DepthPrePass::DepthPrePass(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	this->texture = 0;
	this->framebuffer = 0;

	glGenFramebuffers(1, &framebuffer);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void DepthPrePass::render()
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	Runtime::depthPrePassShader->bind();

	std::vector<EntityProcessor*> entityLinks = Runtime::getEntityLinks();
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->depthPrePass();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthPrePass::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int DepthPrePass::getWidth() const
{
	return width;
}

unsigned int DepthPrePass::getHeight() const
{
	return height;
}

unsigned int DepthPrePass::getFramebuffer() const
{
	return framebuffer;
}