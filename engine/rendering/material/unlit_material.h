#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
#include "imaterial.h"

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial() {

		shader = ShaderBuilder::get("unlit");
		texture = nullptr;
		baseColor = glm::vec4(1.0f);

	}

	void bind() {

		shader->bind();

		if (texture != nullptr) {
			texture->bind();
		}

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

