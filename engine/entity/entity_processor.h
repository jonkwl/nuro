#pragma once

#include "../engine/rendering/core/transformation.h"
#include "../engine/rendering/material/imaterial.h"
#include "../engine/entity/entity.h"

class EntityProcessor
{
public:
	EntityProcessor(Entity* entity);
	void forwardPass();
	void depthPrePass();
	void shadowPass();

	static glm::mat4 currentViewMatrix;
	static glm::mat4 currentProjectionMatrix;
	static glm::mat4 currentLightSpaceMatrix;
private:
	Entity* entity;
};
 