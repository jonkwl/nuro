#pragma once

#include <string>
#include <vector>
#include <glm.hpp>

class Cubemap;
class Shader;

class Skybox
{
public:
	explicit Skybox(Cubemap *cubemap);
	explicit Skybox(Cubemap *cubemap, Shader *custom_shader);

	void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	float emission;

private:
	unsigned int cubemapTexture;
	unsigned int vao;
	unsigned int vbo;

	void generate(Cubemap *cubemap);

	void create_textures(Cubemap *cubemap);
	void create_buffers();

	Shader *shader;
};