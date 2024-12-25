#include "lit_material.h"

#include <glad/glad.h>

#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/utils/log.h"

uint32_t LitMaterial::instances = 0;
Viewport* LitMaterial::viewport = nullptr;
TransformComponent* LitMaterial::cameraTransform = nullptr;
uint32_t LitMaterial::ssaoInput = 0;
PostProcessing::Profile* LitMaterial::profile = nullptr;
bool LitMaterial::castShadows = true;
ShadowDisk* LitMaterial::mainShadowDisk = nullptr;
ShadowMap* LitMaterial::mainShadowMap = nullptr;
glm::mat4 LitMaterial::lightSpace = glm::mat4(1.0f);

LitMaterial::LitMaterial() : baseColor(1.0f),
tiling(1.0f, 1.0f),
offset(0.0f, 0.0f),
roughness(0.0f),
metallic(0.0f),
normalMapIntensity(1.0f),
emission(false),
emissionIntensity(0.0f),
emissionColor(1.0f, 1.0f, 1.0f),
heightMapScale(1.0f),
enableAlbedoMap(false),
albedoMap(Texture::empty()),
enableNormalMap(false),
normalMap(Texture::empty()),
enableRoughnessMap(false),
roughnessMap(Texture::empty()),
enableMetallicMap(false),
metallicMap(Texture::empty()),
enableEmissiveMap(false),
emissiveMap(Texture::empty()),
id(0),
shader(ShaderPool::get("lit")),
shaderId(0)
{
	instances++;

	id = instances;
	shaderId = shader->getId();

	shader->bind();
	syncStaticUniforms();
}

void LitMaterial::bind()
{
	// World parameters
	shader->setMatrix4("lightSpaceMatrix", lightSpace);
	shader->setVec3("configuration.cameraPosition", Transformation::toBackendPosition(cameraTransform->position));

	// General configuration
	shader->setFloat("configuration.gamma", profile->color.gamma);
	shader->setVec2("configuration.viewportResolution", glm::vec2(viewport->width, viewport->height));

	// Shadow parameters
	shader->setBool("configuration.castShadows", castShadows);

	shader->setFloat("configuration.shadowMapResolutionWidth", static_cast<float>(mainShadowMap->getResolutionWidth()));
	shader->setFloat("configuration.shadowMapResolutionHeight", static_cast<float>(mainShadowMap->getResolutionHeight()));

	shader->setFloat("configuration.shadowDiskWindowSize", static_cast<float>(mainShadowDisk->getWindowSize()));
	shader->setFloat("configuration.shadowDiskFilterSize", static_cast<float>(mainShadowDisk->getFilterSize()));
	shader->setFloat("configuration.shadowDiskRadius", static_cast<float>(mainShadowDisk->getRadius()));

	// Bind shadow maps
	mainShadowDisk->bind(SHADOW_DISK_UNIT);
	mainShadowMap->bind(SHADOW_MAP_UNIT);

	// SSAO
	shader->setBool("configuration.enableSSAO", profile->ambientOcclusion.enabled);
	if (profile->ambientOcclusion.enabled) {
		glActiveTexture(GL_TEXTURE0 + SSAO_UNIT);
		glBindTexture(GL_TEXTURE_2D, ssaoInput);
	}

	// Set material data
	shader->setVec4("material.baseColor", baseColor);
	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);
	shader->setBool("material.emission", emission);
	shader->setFloat("material.emissionIntensity", emissionIntensity);
	shader->setVec3("material.emissionColor", emissionColor);

	// Set textures
	shader->setBool("material.enableAlbedoMap", enableAlbedoMap);
	if (enableAlbedoMap)
	{
		albedoMap.bind(ALBEDO_UNIT);
	}

	shader->setBool("material.enableRoughnessMap", enableRoughnessMap);
	if (enableRoughnessMap)
	{
		roughnessMap.bind(ROUGHNESS_UNIT);
	}
	else
	{
		shader->setFloat("material.roughness", roughness);
	}

	shader->setBool("material.enableMetallicMap", enableMetallicMap);
	if (enableMetallicMap)
	{
		metallicMap.bind(METALLIC_UNIT);
	}
	else
	{
		shader->setFloat("material.metallic", metallic);
	}

	shader->setBool("material.enableNormalMap", enableNormalMap);
	if (enableNormalMap)
	{
		normalMap.bind(NORMAL_UNIT);
	}
	shader->setFloat("material.normalMapIntensity", normalMapIntensity);

	shader->setBool("material.enableOcclusionMap", enableOcclusionMap);
	if (enableOcclusionMap)
	{
		occlusionMap.bind(OCCLUSION_UNIT);
	}

	shader->setBool("material.enableEmissiveMap", enableEmissiveMap);
	if (enableEmissiveMap)
	{
		emissiveMap.bind(EMISSIVE_UNIT);
	}

	shader->setBool("material.enableHeightMap", enableHeightMap);
	if (enableHeightMap)
	{
		heightMap.bind(HEIGHT_UNIT);
	}
	shader->setFloat("material.heightMapScale", heightMapScale);
}

uint32_t LitMaterial::getId()
{
	return id;
}

Shader* LitMaterial::getShader()
{
	return shader;
}

uint32_t LitMaterial::getShaderId()
{
	return shaderId;
}

void LitMaterial::setAlbedoMap(Texture texture)
{
	enableAlbedoMap = true;
	albedoMap = texture;
}

void LitMaterial::setRoughnessMap(Texture texture)
{
	enableRoughnessMap = true;
	roughnessMap = texture;
}

void LitMaterial::setMetallicMap(Texture texture)
{
	enableMetallicMap = true;
	metallicMap = texture;
}

void LitMaterial::setNormalMap(Texture texture)
{
	enableNormalMap = true;
	normalMap = texture;
}

void LitMaterial::setOcclusionMap(Texture texture)
{
	enableOcclusionMap = true;
	occlusionMap = texture;
}

void LitMaterial::setEmissiveMap(Texture texture)
{
	enableEmissiveMap = true;
	emissiveMap = texture;
}

void LitMaterial::setHeightMap(Texture texture)
{
	enableHeightMap = true;
	heightMap = texture;
}

void LitMaterial::syncStaticUniforms()
{
	//
	// Sync static texture units
	//

	shader->setInt("material.albedoMap", ALBEDO_UNIT);
	shader->setInt("material.normalMap", NORMAL_UNIT);
	shader->setInt("material.roughnessMap", ROUGHNESS_UNIT);
	shader->setInt("material.metallicMap", METALLIC_UNIT);
	shader->setInt("material.ambientOcclusionMap", OCCLUSION_UNIT);
	shader->setInt("material.emissionMap", EMISSIVE_UNIT);
	shader->setInt("configuration.shadowDisk", SHADOW_DISK_UNIT);
	shader->setInt("configuration.shadowMap", SHADOW_MAP_UNIT);
	shader->setInt("configuration.ssaoBuffer", SSAO_UNIT);

	//
	// Sync scene
	//
	// 
	
	// Fog settings
	shader->setInt("fog.type", 0); // No fog
	// shader->setInt("fog.type", 3);
	// shader->setVec3("fog.color", glm::vec3(1.0f, 1.0f, 1.0f));
	// shader->setFloat("fog.data[0]", 0.01);

	// 
	// Sync lights
	//

	// Lighting parameters
	shader->setInt("configuration.numDirectionalLights", 1);
	shader->setInt("configuration.numPointLights", 1);
	shader->setInt("configuration.numSpotLights", 1);
	
	// Example directional light
	float directionalIntensity = 0.5f;
	glm::vec3 directionalColor = glm::vec3(0.8f, 0.8f, 1.0f);
	glm::vec3 directionalDirection = glm::vec3(-0.7f, -0.8f, 1.0f);
	glm::vec3 directionalPosition = glm::vec3(4.0f, 5.0f, -7.0f);
	shader->setFloat("directionalLights[0].intensity", directionalIntensity);
	shader->setVec3("directionalLights[0].direction", Transformation::toBackendPosition(directionalDirection));
	shader->setVec3("directionalLights[0].color", directionalColor);
	shader->setVec3("directionalLights[0].position", Transformation::toBackendPosition(directionalPosition));
	
	// Example point light
	shader->setVec3("pointLights[0].position", Transformation::toBackendPosition(glm::vec3(0.0f, 6.0f, 3.0f)));
	shader->setVec3("pointLights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("pointLights[0].intensity", 2.0f);
	shader->setFloat("pointLights[0].range", 10.0f);
	shader->setFloat("pointLights[0].falloff", 5.0f);
	
	// Example spot light
	shader->setVec3("spotLights[0].position", Transformation::toBackendPosition(glm::vec3(12.0f, 1.9f, -4.0f)));
	shader->setVec3("spotLights[0].direction", Transformation::toBackendPosition(glm::vec3(-0.4, -0.2f, 1.0f)));
	shader->setVec3("spotLights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
	// shader->setFloat("spotLights[0].intensity", 5.0f);
	shader->setFloat("spotLights[0].intensity", 0.0f);
	shader->setFloat("spotLights[0].range", 25.0f);
	shader->setFloat("spotLights[0].falloff", 10.0f);
	shader->setFloat("spotLights[0].innerCutoff", glm::cos(glm::radians(9.0f)));
	shader->setFloat("spotLights[0].outerCutoff", glm::cos(glm::radians(16.0f)));
}