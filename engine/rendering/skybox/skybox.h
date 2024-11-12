#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

#include "../engine/rendering/core/transformation.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/skybox/cubemap.h"

#include "../engine/utils/log.h"

class Skybox
{
public:
	Skybox(Cubemap* cubemap);
	Skybox(Cubemap* cubemap, Shader* custom_shader);
	
	void render(glm::mat4 view, glm::mat4 projection);

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