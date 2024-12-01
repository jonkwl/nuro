#pragma once

#include "../imaterial.h"

#include "../src/viewport/viewport.h"
#include "../src/camera/camera.h"
#include "../src/rendering/texture/texture.h"

class LitMaterial : public IMaterial
{
public:
	LitMaterial();

	// Needed to be set before binding lit material
	static Viewport* viewport;
	static Camera* camera;

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
	void setAmbientOcclusionMap(Texture texture);
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

	bool enableAmbientOcclusionMap;
	Texture ambientOcclusionMap;

	bool enableEmissionMap;
	Texture emissionMap;

	void syncStaticUniforms();
	void syncLightUniforms();
};