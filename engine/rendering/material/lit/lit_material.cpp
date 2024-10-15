#include "lit_material.h"

#include "../engine/runtime/runtime.h"

LitMaterial::LitMaterial()
{
	shader = ShaderBuilder::get("lit");
	diffuseTexture = Runtime::defaultDiffuseTexture;
	baseColor = glm::vec4(1.0f);
}

void LitMaterial::bind()
{
	shader->bind();

	shader->setVec4("baseColor", baseColor);

	Runtime::mainShadowMap->bind(0);
	shader->setInt("shadowMap", 0);

	diffuseTexture->bind(1);
	shader->setInt("diffuseMap", 1);

	shader->setVec3("cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->position));
	shader->setVec3("lightPosition", Transformation::prepareWorldPosition(Runtime::lightPosition));
	shader->setFloat("lightIntensity", Runtime::lightIntensity);
}

Shader* LitMaterial::getShader()
{
	return shader;
}
