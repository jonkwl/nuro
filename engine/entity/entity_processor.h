#pragma once

#include "../engine/rendering/core/transformation.h"
#include "../engine/entity/entity.h"

class EntityProcessor
{
public:
	EntityProcessor(Entity* entity);
	Entity* entity;
	void render(glm::mat4 view, glm::mat4 projection);
};
