#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <glm.hpp>

#include "../core/rendering/skybox/cubemap.h"

class Shader;

class Skybox
{
public:
	Skybox();
	explicit Skybox(Cubemap& cubemap, Shader* customShader = nullptr);

	void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	float emission;

private:
	uint32_t cubemapTexture;
	uint32_t vao;
	uint32_t vbo;

	void generate(Cubemap& cubemap);

	void createTextures(Cubemap& cubemap);
	void createBuffers();

	Shader* shader;
};