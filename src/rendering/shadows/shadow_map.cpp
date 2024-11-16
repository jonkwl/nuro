#include "shadow_map.h"

#include "../src/runtime/runtime.h"

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
{
	// Set script parameters
	this->width = width;
	this->height = height;

	this->framebuffer = 0;
	this->texture = 0;

	// Generate framebuffer
	glGenFramebuffers(1, &framebuffer);

	// Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Set texture border
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Set framebuffer attachments
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::render()
{
	// Set viewport and bind shadow map framebuffer
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Get shadow map transformation matrices
	glm::mat4 lightProjectionMatrix = Transformation::lightProjectionMatrix(Runtime::getCameraRendering());
	glm::mat4 lightViewMatrix = Transformation::lightViewMatrix(Runtime::directionalPosition, Runtime::directionalDirection);
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
	EntityProcessor::currentLightSpaceMatrix = lightSpaceMatrix;

	// Bind shadow pass shader and render each objects depth on shadow map
	glEnable(GL_DEPTH_TEST);

	// Set culling to front face
	glCullFace(GL_FRONT);

	Runtime::shadowPassShader->bind();

	std::vector<EntityProcessor*> entityLinks = Runtime::getEntityLinks();
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->shadowPass();
	}

	// Unbind shadow map framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int ShadowMap::getWidth() const
{
	return width;
}

unsigned int ShadowMap::getHeight() const
{
	return height;
}

unsigned int ShadowMap::getFramebuffer() const
{
	return framebuffer;
}