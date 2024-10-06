#pragma once

#include "../engine/rendering/core/transformation.h"
#include "../engine/rendering/material/imaterial.h"
#include "../engine/entity/entity.h"

class EntityProcessor
{
public:
	EntityProcessor(Entity* entity);
	void render();
	void shadowPass();

	static void linkDefaults(IMaterial* _defaultMaterial, Shader* _shadowPassShader);

	static glm::mat4 currentView;
	static glm::mat4 currentProjection;
	static glm::mat4 currentLightSpace;
private:
	Entity* entity;

	static IMaterial* defaultMaterial;
	static Shader* shadowPassShader;
};
