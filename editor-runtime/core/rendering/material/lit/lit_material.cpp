#include "lit_material.h"

#include <glad/glad.h>

#include "../core/utils/log.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/transformation/transformation.h"

uint32_t LitMaterial::instances = 0;
Viewport* LitMaterial::viewport = nullptr;
TransformComponent* LitMaterial::cameraTransform = nullptr;
uint32_t LitMaterial::ssaoInput = 0;
PostProcessing::Profile* LitMaterial::profile = nullptr;
bool LitMaterial::castShadows = true;
ShadowDisk* LitMaterial::mainShadowDisk = nullptr;
ShadowMap* LitMaterial::mainShadowMap = nullptr;
glm::mat4 LitMaterial::lightSpace = glm::mat4(1.0f);

LitMaterial::LitMaterial() : baseColor(glm::vec4(1.0f)),
tiling(glm::vec2(1.0f, 1.0f)),
offset(glm::vec2(0.0f, 0.0f)),
roughness(0.0f),
metallic(0.0f),
normalMapIntensity(1.0f),
emission(false),
emissionIntensity(0.0f),
emissionColor(glm::vec3(1.0f)),
heightMapScale(0.1f),
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
enableHeightMap(false),
heightMap(Texture::empty()),
id(0),
shader(ShaderPool::get("lit")),
shaderId(0)
{
	instances++;

	id = instances;
	shaderId = shader->getId();

	shader->bind();
	syncStaticUniforms();
	syncLightUniforms();
}

void LitMaterial::bind()
{
	// World parameters
	shader->setMatrix4("lightSpaceMatrix", lightSpace);
	shader->setVec3("configuration.cameraPosition", Transformation::toBackendPosition(cameraTransform->position));

	// General configuration
	shader->setFloat("configuration.gamma", profile->color.gamma);
	shader->setVec2("configuration.viewportResolution", viewport->getResolution());

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
	shader->setInt("material.emissiveMap", EMISSIVE_UNIT);
	shader->setInt("material.heightMap", HEIGHT_UNIT);
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
}

void LitMaterial::syncLightUniforms()
{
	//
	// Sync lights
	//

	// Fetch lights
	auto directionalLights = ECS::gRegistry.view<TransformComponent, DirectionalLightComponent>();
	auto pointLights = ECS::gRegistry.view<TransformComponent, PointLightComponent>();
	auto spotlights = ECS::gRegistry.view<TransformComponent, SpotlightComponent>();

	// Light count
	size_t nDirectionalLights = 0;
	size_t nPointLights = 0;
	size_t nSpotlights = 0;

	// Limitations
	size_t maxDirectionalLights = 1;
	size_t maxPointLights = 15;
	size_t maxSpotlights = 8;

	// Setup all directional lights
	for (auto [entity, transform, directionalLight] : directionalLights.each()) {
		glm::vec3 directionalDirection = glm::vec3(-0.7f, -0.8f, 1.0f);
		glm::vec3 directionalPosition = glm::vec3(4.0f, 5.0f, -7.0f);

		shader->setFloat(Shader::uniformArray("directionalLights[].intensity", nDirectionalLights), directionalLight.intensity);
		shader->setVec3(Shader::uniformArray("directionalLights[].direction", nDirectionalLights), Transformation::toBackendPosition(directionalDirection));
		shader->setVec3(Shader::uniformArray("directionalLights[].color", nDirectionalLights), directionalLight.color);
		shader->setVec3(Shader::uniformArray("directionalLights[].position", nDirectionalLights), Transformation::toBackendPosition(directionalPosition));

		nDirectionalLights++;
		if (nDirectionalLights >= maxDirectionalLights) break;
	}

	// Setup all point lights
	for (auto [entity, transform, pointLight] : pointLights.each()) {
		shader->setVec3(Shader::uniformArray("pointLights[].position", nPointLights), Transformation::toBackendPosition(transform.position));
		shader->setVec3(Shader::uniformArray("pointLights[].color", nPointLights), pointLight.color);
		shader->setFloat(Shader::uniformArray("pointLights[].intensity", nPointLights), pointLight.intensity);
		shader->setFloat(Shader::uniformArray("pointLights[].range", nPointLights), pointLight.range);
		shader->setFloat(Shader::uniformArray("pointLights[].falloff", nPointLights), pointLight.falloff);

		nPointLights++;
		if (nPointLights >= maxPointLights) break;
	}

	// Setup all spotlights
	for (auto [entity, transform, spotlight] : spotlights.each()) {
		glm::vec3 spotlightDirection = glm::vec3(0.0f, 0.0f, 1.0f);

		shader->setVec3(Shader::uniformArray("spotlights[].position", nSpotlights), Transformation::toBackendPosition(transform.position));
		shader->setVec3(Shader::uniformArray("spotlights[].direction", nSpotlights), Transformation::toBackendPosition(spotlightDirection));
		shader->setVec3(Shader::uniformArray("spotlights[].color", nSpotlights), spotlight.color);
		shader->setFloat(Shader::uniformArray("spotlights[].intensity", nSpotlights), spotlight.intensity);
		shader->setFloat(Shader::uniformArray("spotlights[].range", nSpotlights), spotlight.range);
		shader->setFloat(Shader::uniformArray("spotlights[].falloff", nSpotlights), spotlight.falloff);
		shader->setFloat(Shader::uniformArray("spotlights[].innerCos", nSpotlights), glm::cos(glm::radians(spotlight.innerAngle * 0.5f)));
		shader->setFloat(Shader::uniformArray("spotlights[].outerCos", nSpotlights), glm::cos(glm::radians(spotlight.outerAngle * 0.5f)));

		nSpotlights++;
		if (nSpotlights >= maxSpotlights) break;
	}

	// Lighting parameters
	shader->setInt("configuration.numDirectionalLights", nDirectionalLights);
	shader->setInt("configuration.numPointLights", nPointLights);
	shader->setInt("configuration.numSpotLights", nSpotlights);
}

void LitMaterial::setSampleDirectionalLight()
{
	shader->setInt("configuration.numDirectionalLights", 1);
	shader->setInt("configuration.numPointLights", 0);
	shader->setInt("configuration.numSpotLights", 0);

	size_t index = 0;
	shader->setFloat(Shader::uniformArray("directionalLights[].intensity", index), 1.0f);
	shader->setVec3(Shader::uniformArray("directionalLights[].direction", index), Transformation::toBackendPosition(glm::vec3(-0.5f, -0.5f, 0.5f)));
	shader->setVec3(Shader::uniformArray("directionalLights[].color", index), glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setVec3(Shader::uniformArray("directionalLights[].position", index), Transformation::toBackendPosition(glm::vec3(0.0f, 0.0f, 0.0f)));
}
