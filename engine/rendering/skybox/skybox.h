#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include "../engine/rendering/core/transformation.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

#include "../engine/utils/log.h"

struct CubemapFace {
	std::vector<unsigned char> data;
	int width;
	int height;
	int channels;
};

class Skybox
{
public:
	Skybox(std::string cubemap_source);
	Skybox(std::string cubemap_source, Shader* custom_shader);
	
	void draw(glm::mat4 view, glm::mat4 projection);

	std::string cubemap_source;
	Shader* shader;
private:
	unsigned int texture_id;

	unsigned int vao;
	unsigned int vbo;

	void generate();

	bool create_textures();
	bool create_buffers();

	std::vector<float> get_vertices();

	void extractCubemap(std::string path, std::vector<CubemapFace>& faces);
};