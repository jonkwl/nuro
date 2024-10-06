#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
#include "imaterial.h"

class LitMaterial : public IMaterial
{
public:
	LitMaterial(Texture* diffuseTexture) {

		shader = ShaderBuilder::get("lit");
		this->diffuseTexture = diffuseTexture;
		baseColor = glm::vec4(1.0f);

	}

	void bind() {
		shader->bind();

		shader->setVec4("baseColor", baseColor);

		diffuseTexture->bind(0);
		shader->setInt("diffuseTexture", 0);

		Runtime::bindShadowMap(1);
		shader->setInt("shadowMap", 1);

		shader->setVec3("cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->position));
		shader->setVec3("lightPosition", Transformation::prepareWorldPosition(Runtime::lightPosition));
		shader->setFloat("lightIntensity", Runtime::lightIntensity);
	}

	Shader* getShader() {
		return shader;
	}

	glm::vec4 baseColor;

	Texture* diffuseTexture;
private:
	Shader* shader;
};

