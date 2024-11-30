#pragma once

#include "../imaterial.h"

#include "../src/viewport/viewport.h"
#include "../src/rendering/texture/texture.h"

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial();

	void bind(Viewport& viewport);
	Shader* getShader();

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;

	bool useTexture;
	Texture texture;

private:
	Shader* shader;
};
