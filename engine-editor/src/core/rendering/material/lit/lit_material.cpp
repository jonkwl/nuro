/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!													  !!
   !! 			    BAD TEMPORARY CODE!					  !!
   !!  I am aware this code is so bad its almost spooky.  !!
   !! 			 Please ignore for now. :)				  !!
   !!													  !!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include "lit_material.h"

#include <glad/glad.h>

#include <utils/console.h>
#include <rendering/shadows/shadow_map.h>
#include <rendering/shader/shader_pool.h>
#include <rendering/shadows/shadow_disk.h>
#include <rendering/transformation/transformation.h>

uint32_t LitMaterial::instances = 0;
Viewport* LitMaterial::viewport = nullptr;
TransformComponent* LitMaterial::cameraTransform = nullptr;
uint32_t LitMaterial::ssaoInput = 0;
PostProcessing::Profile* LitMaterial::profile = nullptr;
bool LitMaterial::castShadows = true;
ShadowDisk* LitMaterial::mainShadowDisk = nullptr;
ShadowMap* LitMaterial::mainShadowMap = nullptr;

std::string uniformArray(const std::string& identifier, size_t arrayIndex)
{
	size_t pos = identifier.find("[]");
	if (pos == std::string::npos) return identifier;
	return identifier.substr(0, pos) + "[" + std::to_string(arrayIndex) + "]" + identifier.substr(pos + 2);
}

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
albedoMap(0),
normalMap(0),
roughnessMap(0),
metallicMap(0),
emissiveMap(0),
heightMap(0),
id(0),
shader(ShaderPool::get("lit")),
shaderId(0)
{
	instances++;

	id = instances;
	shaderId = shader->id();

	shader->bind();
	syncStaticUniforms();
	syncLightUniforms();
}

void LitMaterial::bind() const
{
	// Bad temporary code
	if (!shader || !viewport || !cameraTransform || !profile || !mainShadowDisk || !mainShadowMap) return;

	syncLightUniforms();

	// World parameters
	shader->setMatrix4("lightSpaceMatrix", mainShadowMap->getLightSpace());
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
	shader->setBool("material.enableAlbedoMap", albedoMap);
	if (albedoMap)
	{
		glActiveTexture(GL_TEXTURE0 + ALBEDO_UNIT);
		glBindTexture(GL_TEXTURE_2D, albedoMap->id());
	}

	shader->setBool("material.enableRoughnessMap", roughnessMap);
	if (roughnessMap)
	{
		glActiveTexture(GL_TEXTURE0 + ROUGHNESS_UNIT);
		glBindTexture(GL_TEXTURE_2D, roughnessMap->id());
	}
	else
	{
		shader->setFloat("material.roughness", roughness);
	}

	shader->setBool("material.enableMetallicMap", metallicMap);
	if (metallicMap)
	{
		glActiveTexture(GL_TEXTURE0 + METALLIC_UNIT);
		glBindTexture(GL_TEXTURE_2D, metallicMap->id());
	}
	else
	{
		shader->setFloat("material.metallic", metallic);
	}

	shader->setBool("material.enableNormalMap", normalMap);
	if (normalMap)
	{
		glActiveTexture(GL_TEXTURE0 + NORMAL_UNIT);
		glBindTexture(GL_TEXTURE_2D, normalMap->id());
	}
	shader->setFloat("material.normalMapIntensity", normalMapIntensity);
	shader->setBool("material.enableOcclusionMap", occlusionMap);
	/*if (occlusionMap)
	{
		glActiveTexture(GL_TEXTURE0 + OCCLUSION_UNIT);
		glBindTexture(GL_TEXTURE_2D, occlusionMap->id());
	}*/

	shader->setBool("material.enableEmissiveMap", emissiveMap);
	if (emissiveMap)
	{
		glActiveTexture(GL_TEXTURE0 + EMISSIVE_UNIT);
		glBindTexture(GL_TEXTURE_2D, emissiveMap->id());
	}

	shader->setBool("material.enableHeightMap", heightMap);
	if (heightMap)
	{
		glActiveTexture(GL_TEXTURE0 + HEIGHT_UNIT);
		glBindTexture(GL_TEXTURE_2D, heightMap->id());
	}
	shader->setFloat("material.heightMapScale", heightMapScale);
}

uint32_t LitMaterial::getId() const
{
	return id;
}

Shader* LitMaterial::getShader() const
{
	return shader;
}

uint32_t LitMaterial::getShaderId() const
{
	return shaderId;
}

void LitMaterial::syncStaticUniforms() const
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

void LitMaterial::syncLightUniforms() const
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

		if (!directionalLight.enabled) continue;
		shader->setFloat(uniformArray("directionalLights[].intensity", nDirectionalLights), directionalLight.intensity);
		shader->setVec3(uniformArray("directionalLights[].direction", nDirectionalLights), Transformation::toBackendPosition(directionalDirection));
		shader->setVec3(uniformArray("directionalLights[].color", nDirectionalLights), directionalLight.color);
		shader->setVec3(uniformArray("directionalLights[].position", nDirectionalLights), Transformation::toBackendPosition(directionalPosition));

		nDirectionalLights++;
		if (nDirectionalLights >= maxDirectionalLights) break;
	}

	// Setup all point lights
	for (auto [entity, transform, pointLight] : pointLights.each()) {
		if (!pointLight.enabled) continue;
		shader->setVec3(uniformArray("pointLights[].position", nPointLights), Transformation::toBackendPosition(transform.position));
		shader->setVec3(uniformArray("pointLights[].color", nPointLights), pointLight.color);
		shader->setFloat(uniformArray("pointLights[].intensity", nPointLights), pointLight.intensity);
		shader->setFloat(uniformArray("pointLights[].range", nPointLights), pointLight.range);
		shader->setFloat(uniformArray("pointLights[].falloff", nPointLights), pointLight.falloff);

		nPointLights++;
		if (nPointLights >= maxPointLights) break;
	}

	// Setup all spotlights
	for (auto [entity, transform, spotlight] : spotlights.each()) {
		glm::vec3 spotlightDirection = glm::vec3(0.0f, 0.0f, 1.0f);

		if (!spotlight.enabled) continue;
		shader->setVec3(uniformArray("spotlights[].position", nSpotlights), Transformation::toBackendPosition(transform.position));
		shader->setVec3(uniformArray("spotlights[].direction", nSpotlights), Transformation::toBackendPosition(spotlightDirection));
		shader->setVec3(uniformArray("spotlights[].color", nSpotlights), spotlight.color);
		shader->setFloat(uniformArray("spotlights[].intensity", nSpotlights), spotlight.intensity);
		shader->setFloat(uniformArray("spotlights[].range", nSpotlights), spotlight.range);
		shader->setFloat(uniformArray("spotlights[].falloff", nSpotlights), spotlight.falloff);
		shader->setFloat(uniformArray("spotlights[].innerCos", nSpotlights), glm::cos(glm::radians(spotlight.innerAngle * 0.5f)));
		shader->setFloat(uniformArray("spotlights[].outerCos", nSpotlights), glm::cos(glm::radians(spotlight.outerAngle * 0.5f)));

		nSpotlights++;
		if (nSpotlights >= maxSpotlights) break;
	}

	// Lighting parameters
	shader->setInt("configuration.numDirectionalLights", nDirectionalLights);
	shader->setInt("configuration.numPointLights", nPointLights);
	shader->setInt("configuration.numSpotLights", nSpotlights);
}

void LitMaterial::setSampleDirectionalLight() const
{
	shader->setInt("configuration.numDirectionalLights", 1);
	shader->setInt("configuration.numPointLights", 0);
	shader->setInt("configuration.numSpotLights", 0);

	size_t index = 0;
	shader->setFloat(uniformArray("directionalLights[].intensity", index), 1.0f);
	shader->setVec3(uniformArray("directionalLights[].direction", index), Transformation::toBackendPosition(glm::vec3(-0.5f, -0.5f, 0.5f)));
	shader->setVec3(uniformArray("directionalLights[].color", index), glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setVec3(uniformArray("directionalLights[].position", index), Transformation::toBackendPosition(glm::vec3(0.0f, 0.0f, 0.0f)));
}
