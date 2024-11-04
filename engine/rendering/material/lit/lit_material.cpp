#include "lit_material.h"

#include "../engine/runtime/runtime.h"

LitMaterial::LitMaterial()
{
	shader = ShaderBuilder::get("lit");

	baseColor = glm::vec4(1.0f);

	tiling = glm::vec2(1.0f);
	offset = glm::vec2(0.0f);

	albedoMap = nullptr;
	normalMap = nullptr;

	roughness = 0.0f;
	roughnessMap = nullptr;

	metallic = 0.0f;
	metallicMap = nullptr;

	ambientOcclusionMap = nullptr;
}

void LitMaterial::bind()
{
	shader->bind();

	// Set scene data
	Runtime::mainShadowMap->bind(0);
	shader->setInt("scene.shadowMap", 0);
	shader->setVec3("scene.cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->position));
	shader->setVec3("scene.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("scene.ambientStrength", 0.00001f);

	// Set light data
	shader->setVec3("light.position", Transformation::prepareWorldPosition(Runtime::lightPosition));
	shader->setVec3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("light.intensity", Runtime::lightIntensity);

	// Set material data
	shader->setVec4("material.baseColor", baseColor);

	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);

	bool enableAlbedoMap = albedoMap != nullptr;
	shader->setBool("material.enableAlbedoMap", enableAlbedoMap);
	if (enableAlbedoMap) {
		albedoMap->bind(1);
		shader->setInt("material.albedoMap", 1);
	}

	bool enableNormalMap = normalMap != nullptr;
	shader->setBool("material.enableNormalMap", enableNormalMap);
	if (enableNormalMap) {
		normalMap->bind(2);
		shader->setInt("material.normalMap", 2);
	}

	shader->setFloat("material.roughness", roughness);
	bool enableRoughnessMap = roughnessMap != nullptr;
	shader->setBool("material.enableRoughnessMap", enableRoughnessMap);
	if (enableRoughnessMap) {
		roughnessMap->bind(3);
		shader->setInt("material.roughnessMap", 3);
	}

	shader->setFloat("material.metallic", metallic);
	bool enableMetallicMap = metallicMap != nullptr;
	shader->setBool("material.enableMetallicMap", enableMetallicMap);
	if (enableMetallicMap) {
		metallicMap->bind(4);
		shader->setInt("material.metallicMap", 4);
	}

	bool enableAmbientOcclusionMap = ambientOcclusionMap != nullptr;
	shader->setBool("material.enableAmbientOcclusionMap", enableAmbientOcclusionMap);
	if (enableAmbientOcclusionMap) {
		ambientOcclusionMap->bind(5);
		shader->setInt("material.ambientOcclusionMap", 5);
	}
}

Shader* LitMaterial::getShader()
{
	return shader;
}
