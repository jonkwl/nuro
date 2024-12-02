#pragma once

#include "../imaterial.h"

#include "../core/viewport/viewport.h"
#include "../core/rendering/texture/texture.h"

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial();

	void bind();
	Shader* getShader();

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;

	bool useTexture;
	Texture texture;

private:
	Shader* shader;
};
