#include "shadow_map.h"

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>
#include <stb_image_write.h>

#include "../core/utils/console.h"
#include "../core/rendering/model/mesh.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/transformation/transformation.h"

ShadowMap::ShadowMap(uint32_t resolutionWidth, uint32_t resolutionHeight) : resolutionWidth(resolutionWidth),
resolutionHeight(resolutionHeight),
texture(0),
framebuffer(0),
lightSpace(glm::mat4(1.0f)),
shadowPassShader(nullptr)
{
}

void ShadowMap::create()
{
	// Get shader
	shadowPassShader = ShaderPool::get("shadow_pass");

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

	// Check for forward pass framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		Console::out::error("Framebuffer", "Error generating shadow map framebuffer: " + std::to_string(fboStatus));
	}
}

void ShadowMap::destroy()
{
	// Delete texture
	glDeleteTextures(1, &texture);
	texture = 0;

	// Delete framebuffer
	glDeleteFramebuffers(1, &framebuffer);
	framebuffer = 0;

	// Reset light space matrix
	lightSpace = glm::mat4(1.0f);

	// Reset shader
	shadowPassShader = nullptr;
}

void ShadowMap::castShadows(DirectionalLightComponent& directionalLight, TransformComponent& transform, float boundsWidth, float boundsHeight, float near, float far)
{
	glm::vec3 position = glm::vec3(80.0f, 80.0f, -80.0f); // tmp
	glm::vec3 direction = glm::vec3(-0.7f, -0.8f, 1.0f); // tmp

	glm::mat4 view = getView(position, direction);
	glm::mat4 projection = getProjectionOrthographic(boundsWidth, boundsHeight, near, far);

	renderSingular(view, projection);
}

void ShadowMap::castShadows(SpotlightComponent& spotlight, TransformComponent& transform)
{
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f); // tmp

	glm::mat4 view = getView(transform.position, direction);
	glm::mat4 projection = getProjectionPerspective(spotlight.outerAngle, 1.0f, 0.3f, spotlight.range);
	
	renderSingular(view, projection);
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
		Console::out::processDone("Shadow Map", "Shadow map saved as " + filename);
		return true;
	}
	else
	{
		Console::out::error("Shadow Map", "Failed to save shadow map at " + filename);
		return false;
	}
}

void ShadowMap::renderSingular(glm::mat4 view, glm::mat4 projection)
{
	// Calculate light space
	lightSpace = projection * view;

	// Set viewport and bind shadow map framebuffer
	glViewport(0, 0, resolutionWidth, resolutionHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Bind shadow pass shader and render each objects depth on shadow map
	glEnable(GL_DEPTH_TEST);

	// Set culling to front face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	shadowPassShader->bind();

	auto targets = ECS::gRegistry.view<TransformComponent, MeshRendererComponent>();
	for (auto [entity, transform, renderer] : targets.each()) {
		if (!renderer.mesh) return; 

		// Set shadow pass shader uniforms
		shadowPassShader->setMatrix4("modelMatrix", transform.model);
		shadowPassShader->setMatrix4("lightSpaceMatrix", lightSpace);

		// Bind mesh
		glBindVertexArray(renderer.mesh->getVAO());

		// Render mesh
		glDrawElements(GL_TRIANGLES, renderer.mesh->getIndiceCount(), GL_UNSIGNED_INT, 0);
	}

	// Unbind shadow map framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 ShadowMap::getView(const glm::vec3& lightPosition, const glm::vec3& lightDirection) const
{
	// Calculate light view matrix parameters
	glm::vec3 position = Transformation::toBackendPosition(lightPosition);
	glm::vec3 target = position + glm::normalize(Transformation::toBackendPosition(lightDirection));

	// Create and return light view matrix
	return glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 ShadowMap::getProjectionOrthographic(float boundsWidth, float boundsHeight, float near, float far) const
{
	// Create and return light projection matrix using orthographic projection
	return glm::ortho(-boundsWidth * 0.5f, boundsWidth * 0.5f, -boundsHeight * 0.5f, boundsHeight * 0.5f, near, far);
}

glm::mat4 ShadowMap::getProjectionPerspective(float fov, float aspect, float near, float far) const
{
	// Create and return light projection matrix using perspective projection
	return glm::perspective(glm::radians(fov), aspect, near, far);
}