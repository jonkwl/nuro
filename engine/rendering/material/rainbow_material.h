#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
#include "imaterial.h"

class RainbowMaterial : public IMaterial
{
public:
	RainbowMaterial() {

		shader = ShaderBuilder::get("unlit_rainbow");
		texture = nullptr;

	}

	void bind() {

		shader->bind();

		if (texture != nullptr) {
			texture->bind();
		}

	}

	Shader* getShader() {
		return shader;
	}

	Texture* texture;
private:
	Shader* shader;
};

