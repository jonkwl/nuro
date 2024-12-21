#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

#include "../core/rendering/shader/shader.h"

class ShadowMap
{
public:
	explicit ShadowMap(uint32_t resolutionWidth, uint32_t resolutionHeight, float boundsWidth, float boundsHeight, float near, float far);

	void render();
	void bind(uint32_t unit);
	uint32_t getTexture() const;

	uint32_t getResolutionWidth() const;
	uint32_t getResolutionHeight() const;

	float getBoundsWidth() const;
	float getBoundsHeight() const;

	uint32_t getFramebuffer() const;

	const glm::mat4& getLightSpace() const;

private:
	// Shadow map resolution
	uint32_t resolutionWidth;
	uint32_t resolutionHeight;

	// Shadow map projection near and far clipping
	float near;
	float far;

	// Shadow map projection bounds
	float boundsWidth;
	float boundsHeight;

	// Texture and framebuffer
	uint32_t texture;
	uint32_t framebuffer;

	// Cache for latest light space matrix
	glm::mat4 lightSpace;

	// Shadow pass shader
	Shader* shadowPassShader;

private:
	bool saveAsImage(int32_t width, int32_t height, const std::string& filename);
};