#include "shadow_map.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <stb_image_write.h>

#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/model/mesh.h"
#include "../core/utils/log.h"
#include "../core/ecs/ecs_collection.h"

ShadowMap::ShadowMap(uint32_t resolutionWidth, uint32_t resolutionHeight, float boundsWidth, float boundsHeight, float near, float far) : near(near),
far(far),
resolutionWidth(resolutionWidth),
resolutionHeight(resolutionHeight),
boundsWidth(boundsWidth),
boundsHeight(boundsHeight),
framebuffer(0),
texture(0),
lightSpace(glm::mat4(1.0f)),
shadowPassShader(ShaderPool::get("shadow_pass"))
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

void ShadowMap::render()
{
	// Set viewport and bind shadow map framebuffer
	glViewport(0, 0, resolutionWidth, resolutionHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Example directional light
	glm::vec3 directionalDirection = glm::vec3(-0.7f, -0.8f, 1.0f);
	glm::vec3 directionalPosition = glm::vec3(4.0f, 5.0f, -7.0f);

	// Get shadow map view and projection
	glm::mat4 projection = Transformation::lightProjection(boundsWidth, boundsHeight, near, far);
	glm::mat4 view = Transformation::lightView(directionalPosition, directionalDirection);

	// Calculate final light space
	lightSpace = projection * view;

	// Bind shadow pass shader and render each objects depth on shadow map
	glEnable(GL_DEPTH_TEST);

	// Set culling to front face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	shadowPassShader->bind();

	auto targets = ECS::registry.view<TransformComponent, MeshRendererComponent>();
	for (auto [entity, transform, renderer] : targets.each()) {
		// Bind mesh
		glBindVertexArray(renderer.mesh.getVAO());

		// Set shadow pass shader uniforms
		shadowPassShader->setMatrix4("modelMatrix", transform.model);
		shadowPassShader->setMatrix4("lightSpaceMatrix", lightSpace);

		// Render mesh
		glDrawElements(GL_TRIANGLES, renderer.mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
	}

	// Unbind shadow map framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::bind(uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

uint32_t ShadowMap::getTexture() const
{
	return texture;
}

uint32_t ShadowMap::getResolutionWidth() const
{
	return resolutionWidth;
}

uint32_t ShadowMap::getResolutionHeight() const
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

uint32_t ShadowMap::getFramebuffer() const
{
	return framebuffer;
}

const glm::mat4& ShadowMap::getLightSpace() const
{
	return lightSpace;
}

bool ShadowMap::saveAsImage(int32_t width, int32_t height, const std::string& filename)
{
	std::vector<float> depthData(width * height);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, &depthData[0]);
	std::vector<unsigned char> depthImage(width * height);
	for (int32_t i = 0; i < width * height; ++i)
	{
		depthImage[i] = static_cast<unsigned char>(depthData[i] * 255.0f);
	}
	std::vector<unsigned char> flippedImage(width * height);
	for (int32_t y = 0; y < height; ++y)
	{
		memcpy(&flippedImage[y * width], &depthImage[(height - 1 - y) * width], width);
	}
	if (stbi_write_png(filename.c_str(), width, height, 1, &flippedImage[0], width) != 0)
	{
		Log::printProcessDone("Shadow Map", "Shadow map saved as " + filename);
		return true;
	}
	else
	{
		Log::printError("Shadow Map", "Failed to save shadow map at " + filename);
		return false;
	}
}