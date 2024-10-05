#pragma once

#include "../engine/rendering/core/transformation.h"
#include "../engine/entity/entity.h"

class EntityProcessor
{
public:
	EntityProcessor(Entity* entity);
	void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	Entity* entity;
};
