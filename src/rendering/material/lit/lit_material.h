#pragma once

#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_builder.h"
#include "../src/rendering/texture/texture.h"
#include "../src/utils/log.h"
#include "../imaterial.h"

class LitMaterial : public IMaterial
{
public:
	LitMaterial();
	void bind();
	Shader* getShader();

	glm::vec4 baseColor = glm::vec4(1.0f);

	glm::vec2 tiling = glm::vec2(1.0f);
	glm::vec2 offset = glm::vec2(0.0f);

	float roughness = 0.0f;
	float metallic = 0.0f;

	float emissionIntensity;
	glm::vec3 emissionColor;

	void setAlbedoMap(Texture* albedoMap);
	void setNormalMap(Texture* normalMap);
	void setRoughnessMap(Texture* roughnessMap);
	void setMetallicMap(Texture* metallicMap);
	void setAmbientOcclusionMap(Texture* ambientOcclusionMap);
	void setEmissionMap(Texture* emissionMap);
private:
	Shader* shader;

	bool enableAlbedoMap;
	Texture* albedoMap;

	bool enableNormalMap;
	Texture* normalMap;

	bool enableRoughnessMap;
	Texture* roughnessMap;

	bool enableMetallicMap;
	Texture* metallicMap;

	bool enableAmbientOcclusionMap;
	Texture* ambientOcclusionMap;

	bool enableEmissionMap;
	Texture* emissionMap;

	void syncStaticUniforms();
	void syncLightUniforms();
};