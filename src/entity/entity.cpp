#include "entity.h"

#include "../src/runtime/runtime.h"

Entity::Entity()
{
	transform = Transform();

	meshRenderer = nullptr;

	Runtime::linkEntity(this);
}