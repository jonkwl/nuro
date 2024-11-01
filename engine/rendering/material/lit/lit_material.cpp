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

	// Set scene data
	Runtime::mainShadowMap->bind(0);
	shader->setInt("scene.shadowMap", 0);
	shader->setVec3("scene.cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->position));
	shader->setVec3("scene.lightPosition", Transformation::prepareWorldPosition(Runtime::lightPosition));

	// Set light data
	shader->setVec3("light.lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("light.lightIntensity", Runtime::lightIntensity);
	shader->setVec3("light.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("light.ambientIntensity", 0.1f);

	// Set material data
	shader->setVec4("material.baseColor", baseColor);

	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);

	bool enableDiffuseMap = diffuseMap != nullptr;
	shader->setBool("material.enableDiffuseMap", enableDiffuseMap);
	if (enableDiffuseMap) {
		diffuseMap->bind(1);
		shader->setInt("material.diffuseMap", 1);
	}

	bool enableNormalMap = normalMap != nullptr;
	shader->setBool("material.enableNormalMap", enableNormalMap);
	if (enableNormalMap) {
		normalMap->bind(2);
		shader->setInt("material.normalMap", 2);
	}

	shader->setBool("material.useLegacyLighting", true);
}

Shader* LitMaterial::getShader()
{
	return shader;
}
