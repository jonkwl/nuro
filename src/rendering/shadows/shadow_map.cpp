#include "shadow_map.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <stb_image_write.h>

#include "../src/runtime/runtime.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/shader/shader.h"
#include "../src/entity/entity.h"
#include "../src/utils/log.h"

bool shadowMapSaved = false;

void saveDepthMapAsImage(int width, int height, const std::string& filename)
{
	std::vector<float> depthData(width * height);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &depthData[0]);
	std::vector<unsigned char> depthImage(width * height);
	for (int i = 0; i < width * height; ++i)
	{
		depthImage[i] = static_cast<unsigned char>(depthData[i] * 255.0f);
	}
	std::vector<unsigned char> flippedImage(width * height);
	for (int y = 0; y < height; ++y)
	{
		memcpy(&flippedImage[y * width], &depthImage[(height - 1 - y) * width], width);
	}
	if (stbi_write_png(filename.c_str(), width, height, 1, &flippedImage[0], width) != 0)
	{
		Log::printProcessDone("Depth Map", "Depth map saved as " + filename);
	}
	else
	{
		Log::printError("Depth Map", "Failed to save depth map at " + filename);
	}
}

ShadowMap::ShadowMap(unsigned int resolutionWidth, unsigned int resolutionHeight, float boundsWidth, float boundsHeight, float near, float far) : near(near),
far(far),
resolutionWidth(resolutionWidth),
resolutionHeight(resolutionHeight),
boundsWidth(boundsWidth),
boundsHeight(boundsHeight),
framebuffer(0),
texture(0)
{
	// Generate framebuffer
	glGenFramebuffers(1, &framebuffer);

	// Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolutionWidth, resolutionHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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

void ShadowMap::render() // Update needed for camera independence
{
	// Set viewport and bind shadow map framebuffer
	glViewport(0, 0, resolutionWidth, resolutionHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Example directional light
	glm::vec3 directionalDirection = glm::vec3(-0.7f, -0.8f, 1.0f);
	glm::vec3 directionalPosition = glm::vec3(4.0f, 5.0f, -7.0f);

	// Get shadow map transformation matrices
	glm::mat4 lightProjectionMatrix = Transformation::lightProjectionMatrix(Runtime::mainShadowMap->boundsWidth * 2.5f, Runtime::mainShadowMap->boundsHeight * 2.5f, near, far);
	glm::mat4 lightViewMatrix = Transformation::lightViewMatrix(directionalPosition, directionalDirection);
	glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

	// Set mesh renderers light space matrix for upcoming shadow pass
	MeshRenderer::currentLightSpaceMatrix = lightSpaceMatrix;

	// Bind shadow pass shader and render each objects depth on shadow map
	glEnable(GL_DEPTH_TEST);

	// Set culling to front face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Runtime::shadowPassShader->bind();

	std::vector<Entity*> entityStack = Runtime::entityStack;
	for (int i = 0; i < entityStack.size(); i++)
	{
		entityStack[i]->meshRenderer.shadowPass();
	}

	// Unbind shadow map framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Save shadow map (tmp)
	if (!shadowMapSaved)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, getFramebuffer());
		saveDepthMapAsImage(resolutionWidth, resolutionHeight, "./shadow_map.png");
		shadowMapSaved = true;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ShadowMap::bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int ShadowMap::getResolutionWidth() const
{
	return resolutionWidth;
}

unsigned int ShadowMap::getResolutionHeight() const
{
	return resolutionHeight;
}

float ShadowMap::getBoundsWidth() const
{
	return boundsWidth;
}

float ShadowMap::getBoundsHeight() const
{
	return boundsHeight;
}

unsigned int ShadowMap::getFramebuffer() const
{
	return framebuffer;
}