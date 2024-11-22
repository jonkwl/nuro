#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

#include "../src/rendering/core/transformation.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/skybox/cubemap.h"
#include "../src/utils/log.h"

class Skybox
{
public:
	Skybox(Cubemap* cubemap);
	Skybox(Cubemap* cubemap, Shader* custom_shader);
	
	void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	float emission;

	Cubemap* cubemap;
	Shader* shader;
private:
	unsigned int texture_id;

	unsigned int vao;
	unsigned int vbo;

	void generate();

	bool create_textures();
	bool create_buffers();

	std::vector<float> get_vertices();
};