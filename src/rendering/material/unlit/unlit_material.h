#pragma once

#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_builder.h"
#include "../src/rendering/texture/texture.h"
#include "../src/utils/log.h"
#include "../imaterial.h"

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

