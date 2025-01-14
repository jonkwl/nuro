#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

#include "../core/rendering/shader/shader.h"

enum class ShadowType {
	DIRECTIONAL,
	POINT,
	SPOT
};

class ShadowMap
{
public:
	explicit ShadowMap(ShadowType type, uint32_t resolutionWidth, uint32_t resolutionHeight);

	// Set custom bounds for the shadow map
	void setBounds(float boundsWidth, float boundsHeight);

	// Set custom clipping for the shadow map
	void setClipping(float near, float far);

	// Create the shadow map
	void create();

	// Destroy the shadow map
	void destroy();

	// Render the shadow map
	void render();

	// Bind the shadow map texture to a given unit
	void bind(uint32_t unit);

	// Returns the texture of the shadow map
	uint32_t getTexture() const;

	// Returns the resolution width of the shadow map
	uint32_t getResolutionWidth() const;

	// Returns the resolution height of the shadow map
	uint32_t getResolutionHeight() const;

	// Returns the bounds width of the shadow map
	float getBoundsWidth() const;

	// Returns the bounds height of the shadow map
	float getBoundsHeight() const;

	// Returns the framebuffer of the shadow map
	uint32_t getFramebuffer() const;

	// Returns the light space matrix of the shadow map
	const glm::mat4& getLightSpace() const;

	// Saves the latest render of the shadow map as an image
	bool saveAsImage(int32_t width, int32_t height, const std::string& filename);

private:
	// Shadow map type
	ShadowType type;

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
};