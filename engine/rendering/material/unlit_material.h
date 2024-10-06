#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
#include "imaterial.h"

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial(Texture* texture) {

		shader = ShaderBuilder::get("unlit");
		this->texture = texture;
		baseColor = glm::vec4(1.0f);

	}

	void bind() {

		shader->bind();

		texture->bind(0);
		shader->setInt("baseTexture", 0);

		shader->setVec4("baseColor", baseColor);
	}

	Shader* getShader() {
		return shader;
	}

	Texture* texture;
	glm::vec4 baseColor;
private:
	Shader* shader;
};

