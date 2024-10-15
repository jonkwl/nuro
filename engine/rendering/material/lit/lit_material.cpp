#include "lit_material.h"

#include "../engine/runtime/runtime.h"

LitMaterial::LitMaterial()
{
	shader = ShaderBuilder::get("lit");
	baseColor = glm::vec4(1.0f);
	tiling = glm::vec2(1.0f);
	offset = glm::vec2(0.0f);
	diffuseMap = nullptr;
	normalMap = nullptr;
}

void LitMaterial::bind()
{
	shader->bind();

	shader->setVec4("baseColor", baseColor);

	Runtime::mainShadowMap->bind(0);
	shader->setInt("shadowMap", 0);

	shader->setVec2("tiling", tiling);
	shader->setVec2("offset", offset);

	bool enableDiffuseMap = diffuseMap != nullptr;
	shader->setBool("enableDiffuseMap", enableDiffuseMap);
	if (enableDiffuseMap) {
		diffuseMap->bind(1);
		shader->setInt("diffuseMap", 1);
	}

	bool enableNormalMap = normalMap != nullptr;
	shader->setBool("enableNormalMap", enableNormalMap);
	if (enableNormalMap) {
		normalMap->bind(2);
		shader->setInt("normalMap", 2);
	}

	shader->setVec3("cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->position));
	shader->setVec3("lightPosition", Transformation::prepareWorldPosition(Runtime::lightPosition));
	shader->setFloat("lightIntensity", Runtime::lightIntensity);
}

Shader* LitMaterial::getShader()
{
	return shader;
}
