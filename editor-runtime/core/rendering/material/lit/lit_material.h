#pragma once

#include <cstdint>
#include <glm.hpp>

#include "../imaterial.h"

#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/viewport/viewport.h"
#include "../core/rendering/texture/texture.h"
#include "../core/ecs/components.hpp"

class ShadowDisk;
class ShadowMap;

class LitMaterial : public IMaterial
{
public:
	LitMaterial();

	// Needed to be set before binding lit material
	// Replace this through some state object and add a function to push a state
	// Eg "RenderState" and pushRenderState(state)
	// Or rewrite matertial system for more modular and dynamic approach
	static Viewport* viewport;
	static TransformComponent* cameraTransform;
	static uint32_t ssaoInput;
	static PostProcessing::Profile* profile;
	static bool castShadows;
	static ShadowDisk* mainShadowDisk; // tmp until global shadow system
	static ShadowMap* mainShadowMap; // tmp until global shadow system
	static glm::mat4 lightSpace; // tmp until global shadow system

	void bind();
	Shader* getShader();

	glm::vec4 baseColor = glm::vec4(1.0f);

	glm::vec2 tiling = glm::vec2(1.0f);
	glm::vec2 offset = glm::vec2(0.0f);

	float roughness = 0.0f;
	float metallic = 0.0f;

	float normalMapIntensity;

	bool emission;
	float emissionIntensity;
	glm::vec3 emissionColor;

	void setAlbedoMap(Texture texture);
	void setNormalMap(Texture texture);
	void setRoughnessMap(Texture texture);
	void setMetallicMap(Texture texture);
	void setOcclusionMap(Texture texture);
	void setEmissionMap(Texture texture);

private:
	enum TextureUnits
	{
		ALBEDO_UNIT,
		NORMAL_UNIT,
		ROUGHNESS_UNIT,
		METALLIC_UNIT,
		AMBIENT_OCCLUSION_UNIT,
		EMISSIVE_UNIT,
		SHADOW_DISK_UNIT,
		SHADOW_MAP_UNIT,
		SSAO_UNIT
	};

	Shader* shader;

	bool enableAlbedoMap;
	Texture albedoMap;

	bool enableNormalMap;
	Texture normalMap;

	bool enableRoughnessMap;
	Texture roughnessMap;

	bool enableMetallicMap;
	Texture metallicMap;

	bool enableOcclusionMap;
	Texture occlusionMap;

	bool enableEmissionMap;
	Texture emissionMap;

	void syncStaticUniforms();
	void syncLightUniforms();
};