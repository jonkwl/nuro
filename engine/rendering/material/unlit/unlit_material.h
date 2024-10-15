#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
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

