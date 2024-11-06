#include "lit_material.h"

#include "../engine/runtime/runtime.h"

LitMaterial::LitMaterial()
{
	shader = ShaderBuilder::get("lit");

	tiling = glm::vec2(1.0f);
	offset = glm::vec2(0.0f);

	albedoMap = nullptr;
	normalMap = nullptr;

	roughness = 0.0f;
	roughnessMap = nullptr;

	metallic = 0.0f;
	metallicMap = nullptr;

	ambientOcclusionMap = nullptr;

	shader->bind();
	syncStaticUniforms();
	syncSceneUniforms();
	syncLightUniforms();
}

void LitMaterial::bind()
{
	shader->bind();

	// Set scene data
	Runtime::mainShadowMap->bind(SHADOW_MAP_UNIT);
	shader->setVec3("scene.cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->transform.position));

	// Set material data
	shader->setVec4("material.baseColor", baseColor);

	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);

	shader->setBool("material.enableAlbedoMap", enableAlbedoMap);
	if (enableAlbedoMap) {
		albedoMap->bind(ALBEDO_MAP_UNIT);
	}

	shader->setBool("material.enableNormalMap", enableNormalMap);
	if (enableNormalMap) {
		normalMap->bind(NORMAL_MAP_UNIT);
	}

	shader->setBool("material.enableRoughnessMap", enableRoughnessMap);
	if (enableRoughnessMap) {
		roughnessMap->bind(ROUGHNESS_MAP_UNIT);
	}
	else {
		shader->setFloat("material.roughness", roughness);
	}

	shader->setBool("material.enableMetallicMap", enableMetallicMap);
	if (enableMetallicMap) {
		metallicMap->bind(METALLIC_MAP_UNIT);
	}
	else {
		shader->setFloat("material.metallic", metallic);
	}

	shader->setBool("material.enableAmbientOcclusionMap", enableAmbientOcclusionMap);
	if (enableAmbientOcclusionMap) {
		ambientOcclusionMap->bind(AMBIENT_OCCLUSION_MAP_UNIT);
	}
}

Shader* LitMaterial::getShader()
{
	return shader;
}

void LitMaterial::setAlbedoMap(Texture* albedoMap)
{
	enableAlbedoMap = true;
	this->albedoMap = albedoMap;
}

void LitMaterial::setNormalMap(Texture* normalMap)
{
	enableNormalMap = true;
	this->normalMap = normalMap;
}

void LitMaterial::setRoughnessMap(Texture* roughnessMap)
{
	enableRoughnessMap = true;
	this->roughnessMap = roughnessMap;
}

void LitMaterial::setMetallicMap(Texture* metallicMap)
{
	enableMetallicMap = true;
	this->metallicMap = metallicMap;
}

void LitMaterial::setAmbientOcclusionMap(Texture* ambientOcclusionMap)
{
	enableAmbientOcclusionMap = true;
	this->ambientOcclusionMap = ambientOcclusionMap;
}

void LitMaterial::syncStaticUniforms()
{
	shader->setInt("scene.shadowMap", SHADOW_MAP_UNIT);
	shader->setInt("material.albedoMap", ALBEDO_MAP_UNIT);
	shader->setInt("material.normalMap", NORMAL_MAP_UNIT);
	shader->setInt("material.roughnessMap", ROUGHNESS_MAP_UNIT);
	shader->setInt("material.metallicMap", METALLIC_MAP_UNIT);
	shader->setInt("material.ambientOcclusionMap", AMBIENT_OCCLUSION_MAP_UNIT);
}

void LitMaterial::syncSceneUniforms()
{
	shader->setVec3("scene.ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("scene.ambientStrength", 0.00001f);
}

void LitMaterial::syncLightUniforms()
{
	shader->setVec3("light.position", Transformation::prepareWorldPosition(Runtime::lightPosition));
	shader->setVec3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("light.intensity", Runtime::lightIntensity);
}
