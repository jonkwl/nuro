#include "lit_material.h"

#include "../engine/runtime/runtime.h"

glm::vec3 worldPos(glm::vec3 pos) {
	return Transformation::prepareWorldPosition(pos);
}

glm::vec3 eulerAnglesToDirection(float pitch, float yaw, float roll) {
	// Step 1: Create a quaternion from Euler angles
	glm::quat quaternion = glm::quat(glm::vec3(pitch, yaw, roll));

	// Step 2: Define the default forward direction vector
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);

	// Step 3: Rotate the forward vector by the quaternion
	glm::vec3 direction = quaternion * forward;

	return direction;
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

	shader->setFloat("configuration.gamma", PostProcessing::setup.gamma);

	shader->setBool("configuration.solidMode", Runtime::solidMode);

	shader->setBool("configuration.castShadows", Runtime::shadows);

	shader->setVec3("configuration.cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering()->transform.position));

	// Set material data
	shader->setVec4("material.baseColor", baseColor);

	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);

	shader->setBool("material.enableAlbedoMap", enableAlbedoMap);
	if (enableAlbedoMap) {
		albedoMap->bind(ALBEDO_MAP_UNIT);
	}

	shader->setBool("material.enableNormalMap", enableNormalMap && Runtime::normalMapping);
	if (enableNormalMap && Runtime::normalMapping) {
		normalMap->bind(NORMAL_MAP_UNIT);
	}
	shader->setFloat("material.normalMapIntensity", Runtime::normalMappingIntensity);

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

	// tmp debug
	shader->setFloat("directionalLights[0].intensity", Runtime::directionalIntensity);
	shader->setFloat("pointLights[0].intensity", Runtime::intensity);
	shader->setFloat("pointLights[0].range", Runtime::range);
	shader->setFloat("pointLights[0].falloff", Runtime::falloff);
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
	shader->setFloat("configuration.gamma", PostProcessing::setup.gamma);

	shader->setBool("configuration.solidMode", Runtime::solidMode);
	shader->setBool("configuration.castShadows", Runtime::shadows);

	shader->setInt("configuration.shadowMap", SHADOW_MAP_UNIT);
	shader->setInt("material.albedoMap", ALBEDO_MAP_UNIT);
	shader->setInt("material.normalMap", NORMAL_MAP_UNIT);
	shader->setInt("material.roughnessMap", ROUGHNESS_MAP_UNIT);
	shader->setInt("material.metallicMap", METALLIC_MAP_UNIT);
	shader->setInt("material.ambientOcclusionMap", AMBIENT_OCCLUSION_MAP_UNIT);
}

void LitMaterial::syncLightUniforms()
{
	shader->setInt("configuration.numDirectionalLights", 1);
	shader->setInt("configuration.numPointLights", 1);
	shader->setInt("configuration.numSpotLights", 1);

	shader->setFloat("ambientLighting.intensity", 0.008f);
	shader->setVec3("ambientLighting.color", glm::vec3(1.0f, 1.0f, 1.0f));

	shader->setFloat("directionalLights[0].intensity", Runtime::directionalIntensity);
	shader->setVec3("directionalLights[0].direction", worldPos(Runtime::directionalDirection));
	shader->setVec3("directionalLights[0].color", Runtime::directionalColor);
	shader->setVec3("directionalLights[0].position", worldPos(Runtime::directionalPosition));

	shader->setVec3("pointLights[0].position", worldPos(glm::vec3(0.0f, 2.0f, 4.0f)));
	shader->setVec3("pointLights[0].color", glm::vec3(0.0f, 0.6f, 1.0f));
	shader->setFloat("pointLights[0].intensity", 1.5f);
	shader->setFloat("pointLights[0].range", 10.0f);
	shader->setFloat("pointLights[0].falloff", 5.0f);

	shader->setVec3("spotLights[0].position", worldPos(glm::vec3(12.0f, 1.9f, -4.0f)));
	shader->setVec3("spotLights[0].direction", worldPos(glm::vec3(-0.4, -0.2f, 1.0f)));
	shader->setVec3("spotLights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("spotLights[0].intensity", 3.5f);
	shader->setFloat("spotLights[0].range", 25.0f);
	shader->setFloat("spotLights[0].falloff", 10.0f);
	shader->setFloat("spotLights[0].innerCutoff", glm::cos(glm::radians(12.5f)));
	shader->setFloat("spotLights[0].outerCutoff", glm::cos(glm::radians(20.0f)));

	shader->setInt("fog.type", 0);
	/*shader->setInt("fog.type", 3);
	shader->setVec3("fog.color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("fog.data[0]", 0.01);*/
}
