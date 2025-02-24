#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "../imaterial.h"

#include <viewport/viewport.h>
#include <ecs/ecs_collection.h>
#include <rendering/texture/texture.h>
#include <rendering/postprocessing/post_processing.h>

class ShadowDisk;
class ShadowMap;

class LitMaterial : public IMaterial
{
public:
	LitMaterial();

	void bind() const override;
	uint32_t getId() const override;
	Shader* getShader() const override;
	uint32_t getShaderId() const override;

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;
	float roughness;
	float metallic;
	float normalMapIntensity;
	bool emission;
	float emissionIntensity;
	glm::vec3 emissionColor;
	float heightMapScale;

	Texture* albedoMap;
	Texture* roughnessMap;
	Texture* metallicMap;
	Texture* normalMap;
	Texture* occlusionMap;
	Texture* emissiveMap;
	Texture* heightMap;

	void syncStaticUniforms() const;
	void syncLightUniforms() const;
	void setSampleDirectionalLight() const;

public:
	// Instance counter
	static uint32_t instances;

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

private:
	enum TextureUnits
	{
		ALBEDO_UNIT,
		NORMAL_UNIT,
		ROUGHNESS_UNIT,
		METALLIC_UNIT,
		OCCLUSION_UNIT,
		EMISSIVE_UNIT,
		HEIGHT_UNIT,
		SHADOW_DISK_UNIT,
		SHADOW_MAP_UNIT,
		SSAO_UNIT
	};

	uint32_t id;
	Shader* shader;
	uint32_t shaderId;
};