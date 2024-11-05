#pragma once

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/utils/log.h"
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

	void setAlbedoMap(Texture* albedoMap);
	void setNormalMap(Texture* normalMap);
	void setRoughnessMap(Texture* roughnessMap);
	void setMetallicMap(Texture* metallicMap);
	void setAmbientOcclusionMap(Texture* ambientOcclusionMap);
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

	void syncStaticUniforms();
	void syncSceneUniforms();
	void syncLightUniforms();
};