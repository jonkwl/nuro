#pragma once

#include "../src/rendering/core/transformation.h"
#include "../src/rendering/material/imaterial.h"
#include "../src/entity/entity.h"

class EntityProcessor
{
public:
	EntityProcessor(Entity* entity);
	void forwardPass();
	void prePass();
	void shadowPass();

	static glm::mat4 currentViewMatrix;
	static glm::mat4 currentProjectionMatrix;
	static glm::mat4 currentLightSpaceMatrix;
private:
	Entity* entity;
};
 