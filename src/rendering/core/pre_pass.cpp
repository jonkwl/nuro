#include "pre_pass.h"

#include "../src/runtime/runtime.h"

unsigned int PrePass::width = 0;
unsigned int PrePass::height = 0;

unsigned int PrePass::texture = 0;
unsigned int PrePass::framebuffer = 0;

void PrePass::setup(unsigned int width, unsigned int height)
{
	PrePass::width = width;
	PrePass::height = height;

	PrePass::texture = 0;
	PrePass::framebuffer = 0;

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

void PrePass::render()
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	Runtime::prePassShader->bind();

	std::vector<Entity*> entityLinks = Runtime::getEntityLinks();
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->meshRenderer->prePass();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PrePass::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int PrePass::getWidth()
{
	return width;
}

unsigned int PrePass::getHeight()
{
	return height;
}

unsigned int PrePass::getFramebuffer()
{
	return framebuffer;
}