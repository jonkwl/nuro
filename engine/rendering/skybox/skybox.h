#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <stb_image.h>

#include "../engine/rendering/core/transformation.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

#include "../engine/utils/log.h"

class Skybox
{
public:
	Skybox(std::vector<std::string> sources);
	Skybox(std::vector<std::string> sources, Shader* custom_shader);
	
	void draw(glm::mat4 view, glm::mat4 projection);

	std::vector<std::string> sources;
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