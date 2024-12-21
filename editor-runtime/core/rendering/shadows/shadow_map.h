#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/rendering/shader/shader.h"

class ShadowMap
{
public:
	explicit ShadowMap(unsigned int resolutionWidth, unsigned int resolutionHeight, float boundsWidth, float boundsHeight, float near, float far);

	void render();
	void bind(unsigned int unit);
	unsigned int getTexture() const;

	unsigned int getResolutionWidth() const;
	unsigned int getResolutionHeight() const;

	float getBoundsWidth() const;
	float getBoundsHeight() const;

	unsigned int getFramebuffer() const;

	const glm::mat4& getLightSpace() const;

private:
	// Shadow map resolution
	unsigned int resolutionWidth;
	unsigned int resolutionHeight;

	// Shadow map projection near and far clipping
	float near;
	float far;

	// Shadow map projection bounds
	float boundsWidth;
	float boundsHeight;

	// Texture and framebuffer
	unsigned int texture;
	unsigned int framebuffer;

	// Cache for latest light space matrix
	glm::mat4 lightSpace;

	// Shadow pass shader
	Shader* shadowPassShader;

private:
	bool saveAsImage(int width, int height, const std::string& filename);
};