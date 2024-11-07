#include "lit_material.h"

#include "../engine/runtime/runtime.h"

glm::vec3 worldPos(glm::vec3 pos) {
	return Transformation::prepareWorldPosition(pos);
}

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

	shader->setFloat("directionalLight.intensity", Runtime::directionalIntensity);
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

void LitMaterial::syncLightUniforms()
{
	shader->setInt("scene.numDirectionalLights", 1);
	shader->setInt("scene.numPointLights", 1);

	shader->setFloat("ambientLighting.intensity", 0.00001f);
	shader->setVec3("ambientLighting.color", glm::vec3(1.0f, 1.0f, 1.0f));

	shader->setFloat("directionalLights[0].intensity", Runtime::directionalIntensity);
	shader->setVec3("directionalLights[0].color", Runtime::directionalColor);
	shader->setVec3("directionalLights[0].direction", worldPos(Runtime::directionalDirection));
	shader->setVec3("directionalLights[0].position", worldPos(Runtime::directionalPosition));

	shader->setVec3("pointLights[0].position", worldPos(glm::vec3(0.0f, 1.0f, 1.0f)));
	shader->setVec3("pointLights[0].color", glm::vec3(0.0f, 0.4f, 0.8f));
	shader->setFloat("pointLights[0].intensity", 1.0f);
}
