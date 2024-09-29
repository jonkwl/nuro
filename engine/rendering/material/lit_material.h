#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
#include "imaterial.h"

class LitMaterial : public IMaterial
{
public:
	LitMaterial(Texture* texture) {

		shader = ShaderBuilder::get("lit");
		this->texture = texture;
		baseColor = glm::vec4(1.0f);

	}

	void bind() {
		shader->bind();
		texture->bind();
		shader->setVec4("baseColor", baseColor);
		shader->setVec3("cameraPosition", Runtime::renderCamera->position);
		shader->setVec3("lightPosition", lightPosition);
	}

	Shader* getShader() {
		return shader;
	}

	Texture* texture;
	glm::vec4 baseColor;
	glm::vec3 lightPosition;
private:
	Shader* shader;
};

