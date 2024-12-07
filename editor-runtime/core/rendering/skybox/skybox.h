#pragma once

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

	void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	float emission;

private:
	unsigned int cubemapTexture;
	unsigned int vao;
	unsigned int vbo;

	void generate(Cubemap& cubemap);

	void createTextures(Cubemap& cubemap);
	void createBuffers();

	Shader* shader;
};