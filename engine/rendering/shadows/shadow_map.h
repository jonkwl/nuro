#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <vector>

#include "../engine/rendering/shader/Shader.h"

class ShadowMap
{
public:
	ShadowMap(unsigned int size, Shader* shader);

	void render(glm::vec3 lightPosition, glm::vec3 lightDirection);
	void bind(unsigned int slot);

	unsigned int getSize();
	unsigned int getFramebuffer();
private:
	unsigned int size;
	Shader* shader;

	unsigned int texture;
	unsigned int framebuffer;
};