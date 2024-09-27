#pragma once

#include "../engine/rendering/core/render_core.h"
#include "../engine/objects/entity.h"

class RuntimeEntity
{
public:
	RuntimeEntity(Entity* entity);
	Entity* entity;
	void render();
};
