#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include <memory/resource_manager.h>
#include <rendering/skybox/cubemap.h>

class Shader;

class Skybox
{
public:
	Skybox();

	// Sets the cubemap of the skybox
	void setCubemap(ResourceRef<Cubemap> cubemap);

	// Returns the cubemap of the skybox
	ResourceRef<Cubemap> getCubemap() const;

	// Sets the emission of the skybox
	void setEmission(float emission);

	// Returns the current emission of the skybox
	float getEmission() const;

	// Generates the skybox
	void create();

	// Renders the skybox with the given transformation matrices
	void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
	ResourceRef<Cubemap> cubemap;
	ResourceRef<Shader> shader;
	uint32_t vao;
	uint32_t vbo;
	float emission;
};