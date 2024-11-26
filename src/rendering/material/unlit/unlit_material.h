#pragma once

#include "../imaterial.h"

class Texture;

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial();
	void bind();
	Shader* getShader();

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;
	Texture* texture;
private:
	Shader* shader;
};

