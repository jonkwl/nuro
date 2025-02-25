#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <ecs/ecs_collection.h>
#include <rendering/shader/shader.h>

class ShadowMap
{
public:
	explicit ShadowMap(uint32_t resolutionWidth, uint32_t resolutionHeight);

	// Create the shadow map
	void create();

	// Destroy the shadow map
	void destroy();

	// Renders the shadow map for a directional light source
	void castShadows(DirectionalLightComponent& directionalLight, TransformComponent& transform, float boundsWidth, float boundsHeight, float near, float far);

	// Renders the shadow map for a spotlight source
	void castShadows(SpotlightComponent& spotlight, TransformComponent& transform);

	// Bind the shadow map texture to a given unit
	void bind(uint32_t unit);

	// Returns the texture of the shadow map
	uint32_t getTexture() const;

	// Returns the resolution width of the shadow map
	uint32_t getResolutionWidth() const;

	// Returns the resolution height of the shadow map
	uint32_t getResolutionHeight() const;

	// Returns the framebuffer of the shadow map
	uint32_t getFramebuffer() const;

	// Returns the light space matrix of the shadow map
	const glm::mat4& getLightSpace() const;

	// Saves the latest render of the shadow map as an image
	bool saveAsImage(int32_t width, int32_t height, const std::string& filename);

private:
	// Render onto singular texture
	void renderSingular(glm::mat4 view, glm::mat4 projection);

	// Returns a view matrix for a light
	glm::mat4 getView(const glm::vec3& lightPosition, const glm::vec3& lightDirection) const;

	// Returns an orthograpghic projection matrix for a light
	glm::mat4 getProjectionOrthographic(float boundsWidth, float boundsHeight, float near, float far) const;

	// Returns a perspective projection matrix for a light
	glm::mat4 getProjectionPerspective(float fov, float aspect, float near, float far) const;

	// Shadow map resolution width
	uint32_t resolutionWidth;

	// Shadow map resolution height
	uint32_t resolutionHeight;

	// Shadow map backend texture id
	uint32_t texture;

	// Shadow map backend framebuffer id
	uint32_t framebuffer;

	// Cache for latest light space matrix
	glm::mat4 lightSpace;

	// Shadow pass shader
	Shader* shadowPassShader;
};