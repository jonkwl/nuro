#include "lit_material.h"

#include <glad/glad.h>

#include "../src/runtime/runtime.h"
#include "../src/camera/camera.h"
#include "../src/rendering/shadows/shadow_map.h"
#include "../src/rendering/shadows/shadow_disk.h"
#include "../src/rendering/core/transformation.h"
#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"

LitMaterial::LitMaterial() : shader(ShaderPool::get("lit")),
							 tiling(1.0f, 1.0f),
							 offset(0.0f, 0.0f),
							 roughness(0.0f),
							 metallic(0.0f),
							 emission(false),
							 emissionIntensity(0.0f),
							 emissionColor(1.0f, 1.0f, 1.0f),
							 enableAlbedoMap(false),
							 albedoMap(),
							 enableNormalMap(false),
							 normalMap(),
							 enableRoughnessMap(false),
							 roughnessMap(),
							 enableMetallicMap(false),
							 metallicMap(),
							 enableEmissionMap(false),
							 emissionMap()
{
	shader->bind();
	syncStaticUniforms();
	syncLightUniforms();
}

void LitMaterial::bind()
{
	shader->bind();

	// General parameters
	shader->setFloat("configuration.gamma", PostProcessing::configuration.gamma);
	shader->setBool("configuration.solidMode", Runtime::solidMode);

	// Shadow parameters
	shader->setBool("configuration.castShadows", Runtime::shadows);
	Runtime::mainShadowMap->bind(SHADOW_MAP_UNIT);
	shader->setFloat("configuration.shadowMapResolutionWidth", static_cast<float>(Runtime::mainShadowMap->getResolutionWidth()));
	shader->setFloat("configuration.shadowMapResolutionHeight", static_cast<float>(Runtime::mainShadowMap->getResolutionHeight()));

	Runtime::mainShadowDisk->bind(SHADOW_DISK_UNIT);
	shader->setFloat("configuration.shadowDiskWindowSize", static_cast<float>(Runtime::mainShadowDisk->getWindowSize()));
	shader->setFloat("configuration.shadowDiskFilterSize", static_cast<float>(Runtime::mainShadowDisk->getFilterSize()));
	shader->setFloat("configuration.shadowDiskRadius", static_cast<float>(Runtime::mainShadowDisk->getRadius()));

	// SSAO
	shader->setBool("configuration.enableSSAO", PostProcessing::configuration.ambientOcclusion);
	glActiveTexture(GL_TEXTURE0 + SSAO_UNIT);
	glBindTexture(GL_TEXTURE_2D, Runtime::ssaoBuffer);

	// World parameters
	shader->setVec3("configuration.cameraPosition", Transformation::prepareWorldPosition(Runtime::getCameraRendering().transform.position));

	// Set material data
	shader->setVec4("material.baseColor", baseColor);

	shader->setVec2("material.tiling", tiling);
	shader->setVec2("material.offset", offset);

	shader->setBool("material.emission", emission);
	shader->setFloat("material.emissionIntensity", emissionIntensity);
	shader->setVec3("material.emissionColor", emissionColor);

	shader->setBool("material.enableAlbedoMap", enableAlbedoMap);
	if (enableAlbedoMap)
	{
		albedoMap.bind(ALBEDO_UNIT);
	}

	shader->setBool("material.enableNormalMap", enableNormalMap && Runtime::normalMapping);
	if (enableNormalMap && Runtime::normalMapping)
	{
		normalMap.bind(NORMAL_UNIT);
	}
	shader->setFloat("material.normalMapIntensity", Runtime::normalMappingIntensity);

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

	shader->setBool("material.enableAmbientOcclusionMap", enableAmbientOcclusionMap);
	if (enableAmbientOcclusionMap)
	{
		ambientOcclusionMap.bind(AMBIENT_OCCLUSION_UNIT);
	}

	shader->setBool("material.enableEmissionMap", enableEmissionMap);
	if (enableEmissionMap)
	{
		emissionMap.bind(EMISSIVE_UNIT);
	}
}

Shader *LitMaterial::getShader()
{
	return shader;
}

void LitMaterial::setAlbedoMap(Texture texture)
{
	enableAlbedoMap = true;
	albedoMap = texture;
}

void LitMaterial::setNormalMap(Texture texture)
{
	enableNormalMap = true;
	normalMap = texture;
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

void LitMaterial::setAmbientOcclusionMap(Texture texture)
{
	enableAmbientOcclusionMap = true;
	ambientOcclusionMap = texture;
}

void LitMaterial::setEmissionMap(Texture texture)
{
	enableEmissionMap = true;
	emissionMap = texture;
}

void LitMaterial::syncStaticUniforms()
{
	shader->setFloat("configuration.gamma", PostProcessing::configuration.gamma);
	shader->setVec2("configuration.screenResolution", glm::vec2(Window::width, Window::height));

	shader->setBool("configuration.solidMode", Runtime::solidMode);
	shader->setBool("configuration.castShadows", Runtime::shadows);

	shader->setInt("material.albedoMap", ALBEDO_UNIT);
	shader->setInt("material.normalMap", NORMAL_UNIT);
	shader->setInt("material.roughnessMap", ROUGHNESS_UNIT);
	shader->setInt("material.metallicMap", METALLIC_UNIT);
	shader->setInt("material.ambientOcclusionMap", AMBIENT_OCCLUSION_UNIT);
	shader->setInt("material.emissionMap", EMISSIVE_UNIT);
	shader->setInt("configuration.shadowDisk", SHADOW_DISK_UNIT);
	shader->setInt("configuration.shadowMap", SHADOW_MAP_UNIT);
	shader->setInt("configuration.ssaoBuffer", SSAO_UNIT);
}

void LitMaterial::syncLightUniforms()
{
	// Lighting parameters
	shader->setInt("configuration.numDirectionalLights", 1);
	shader->setInt("configuration.numPointLights", 15);
	shader->setInt("configuration.numSpotLights", 1);

	shader->setFloat("directionalLights[0].intensity", Runtime::directionalIntensity);
	shader->setVec3("directionalLights[0].direction", Transformation::prepareWorldPosition(Runtime::directionalDirection));
	shader->setVec3("directionalLights[0].color", Runtime::directionalColor);
	shader->setVec3("directionalLights[0].position", Transformation::prepareWorldPosition(Runtime::directionalPosition));

	shader->setVec3("pointLights[0].position", Transformation::prepareWorldPosition(glm::vec3(0.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[0].color", glm::vec3(0.0f, 0.78f, 0.95f));
	shader->setFloat("pointLights[0].intensity", 0.25f);
	shader->setFloat("pointLights[0].range", 10.0f);
	shader->setFloat("pointLights[0].falloff", 5.0f);

	shader->setVec3("spotLights[0].position", Transformation::prepareWorldPosition(glm::vec3(12.0f, 1.9f, -4.0f)));
	shader->setVec3("spotLights[0].direction", Transformation::prepareWorldPosition(glm::vec3(-0.4, -0.2f, 1.0f)));
	shader->setVec3("spotLights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("spotLights[0].intensity", 5.0);
	shader->setFloat("spotLights[0].range", 25.0f);
	shader->setFloat("spotLights[0].falloff", 10.0f);
	shader->setFloat("spotLights[0].innerCutoff", glm::cos(glm::radians(9.0f)));
	shader->setFloat("spotLights[0].outerCutoff", glm::cos(glm::radians(16.0f)));

	shader->setInt("fog.type", 0);
	/*shader->setInt("fog.type", 3);
	shader->setVec3("fog.color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader->setFloat("fog.data[0]", 0.01);*/

	shader->setVec3("pointLights[1].position", Transformation::prepareWorldPosition(glm::vec3(4.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[1].color", glm::vec3(0.78f, 0.0f, 0.95f));
	shader->setFloat("pointLights[1].intensity", 0.25f);
	shader->setFloat("pointLights[1].range", 10.0f);
	shader->setFloat("pointLights[1].falloff", 5.0f);

	shader->setVec3("pointLights[2].position", Transformation::prepareWorldPosition(glm::vec3(8.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[2].color", glm::vec3(0.2f, 0.8f, 0.3f));
	shader->setFloat("pointLights[2].intensity", 0.25f);
	shader->setFloat("pointLights[2].range", 10.0f);
	shader->setFloat("pointLights[2].falloff", 5.0f);

	shader->setVec3("pointLights[3].position", Transformation::prepareWorldPosition(glm::vec3(12.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[3].color", glm::vec3(0.6f, 0.2f, 0.9f));
	shader->setFloat("pointLights[3].intensity", 0.05f);
	shader->setFloat("pointLights[3].range", 10.0f);
	shader->setFloat("pointLights[3].falloff", 5.0f);

	shader->setVec3("pointLights[4].position", Transformation::prepareWorldPosition(glm::vec3(16.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[4].color", glm::vec3(0.1f, 0.35f, 0.7f));
	shader->setFloat("pointLights[4].intensity", 0.05f);
	shader->setFloat("pointLights[4].range", 10.0f);
	shader->setFloat("pointLights[4].falloff", 5.0f);

	shader->setVec3("pointLights[5].position", Transformation::prepareWorldPosition(glm::vec3(20.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[5].color", glm::vec3(0.78f, 0.0f, 0.95f));
	shader->setFloat("pointLights[5].intensity", 0.05f);
	shader->setFloat("pointLights[5].range", 10.0f);
	shader->setFloat("pointLights[5].falloff", 5.0f);

	shader->setVec3("pointLights[6].position", Transformation::prepareWorldPosition(glm::vec3(24.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[6].color", glm::vec3(0.2f, 0.8f, 0.3f));
	shader->setFloat("pointLights[6].intensity", 0.05f);
	shader->setFloat("pointLights[6].range", 10.0f);
	shader->setFloat("pointLights[6].falloff", 5.0f);

	shader->setVec3("pointLights[7].position", Transformation::prepareWorldPosition(glm::vec3(28.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[7].color", glm::vec3(0.6f, 0.2f, 0.9f));
	shader->setFloat("pointLights[7].intensity", 4.0f);
	shader->setFloat("pointLights[7].range", 3.0f);
	shader->setFloat("pointLights[7].falloff", 5.0f);

	shader->setVec3("pointLights[8].position", Transformation::prepareWorldPosition(glm::vec3(32.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[8].color", glm::vec3(0.1f, 0.35f, 0.7f));
	shader->setFloat("pointLights[8].intensity", 4.0f);
	shader->setFloat("pointLights[8].range", 10.0f);
	shader->setFloat("pointLights[8].falloff", 5.0f);

	shader->setVec3("pointLights[9].position", Transformation::prepareWorldPosition(glm::vec3(36.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[9].color", glm::vec3(0.78f, 0.0f, 0.95f));
	shader->setFloat("pointLights[9].intensity", 2.0f);
	shader->setFloat("pointLights[9].range", 10.0f);
	shader->setFloat("pointLights[9].falloff", 5.0f);

	shader->setVec3("pointLights[10].position", Transformation::prepareWorldPosition(glm::vec3(40.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[10].color", glm::vec3(0.2f, 0.8f, 0.3f));
	shader->setFloat("pointLights[10].intensity", 1.0f);
	shader->setFloat("pointLights[10].range", 10.0f);
	shader->setFloat("pointLights[10].falloff", 5.0f);

	shader->setVec3("pointLights[11].position", Transformation::prepareWorldPosition(glm::vec3(44.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[11].color", glm::vec3(0.6f, 0.2f, 0.9f));
	shader->setFloat("pointLights[11].intensity", 3.0f);
	shader->setFloat("pointLights[11].range", 10.0f);
	shader->setFloat("pointLights[11].falloff", 5.0f);

	shader->setVec3("pointLights[12].position", Transformation::prepareWorldPosition(glm::vec3(48.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[12].color", glm::vec3(0.1f, 0.35f, 0.7f));
	shader->setFloat("pointLights[12].intensity", 4.0f);
	shader->setFloat("pointLights[12].range", 10.0f);
	shader->setFloat("pointLights[12].falloff", 5.0f);

	shader->setVec3("pointLights[13].position", Transformation::prepareWorldPosition(glm::vec3(52.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[13].color", glm::vec3(0.78f, 0.0f, 0.95f));
	shader->setFloat("pointLights[13].intensity", 2.0f);
	shader->setFloat("pointLights[13].range", 10.0f);
	shader->setFloat("pointLights[13].falloff", 5.0f);

	shader->setVec3("pointLights[14].position", Transformation::prepareWorldPosition(glm::vec3(56.0f, 0.0f, 6.5f)));
	shader->setVec3("pointLights[14].color", glm::vec3(0.2f, 0.8f, 0.3f));
	shader->setFloat("pointLights[14].intensity", 1.0f);
	shader->setFloat("pointLights[14].range", 10.0f);
	shader->setFloat("pointLights[14].falloff", 5.0f);
}
