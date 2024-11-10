#include "shadow_map.h"

#include "../engine/runtime/runtime.h"

ShadowMap::ShadowMap(unsigned int size)
{
	this->size = size;

	this->framebuffer = 0;
	this->texture = 0;

	glGenFramebuffers(1, &framebuffer);

	const unsigned int SHADOW_MAP_WIDTH = size, SHADOW_MAP_HEIGHT = size;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::render()
{
	// Set viewport and bind shadow map framebuffer
	glViewport(0, 0, size, size);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Get shadow map transformation matrices
	glm::mat4 lightProjectionMatrix = Transformation::lightProjectionMatrix(Runtime::getCameraRendering());
	glm::mat4 lightViewMatrix = Transformation::lightViewMatrix(Runtime::directionalPosition, Runtime::directionalDirection);
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
	EntityProcessor::currentLightSpaceMatrix = lightSpaceMatrix;

	// Bind shadow pass shader and render each objects depth on shadow map
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	Runtime::shadowPassShader->bind();

	std::vector<EntityProcessor*> entityLinks = Runtime::getEntityLinks();
	for (int i = 0; i < entityLinks.size(); i++) {
		entityLinks.at(i)->shadowPass();
	}
	glCullFace(GL_BACK);

	// Unbind shadow map framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::bind(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int ShadowMap::getSize()
{
	return size;
}

unsigned int ShadowMap::getFramebuffer()
{
	return framebuffer;
}