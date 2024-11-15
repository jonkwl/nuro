#include "entity.h"

#include "../src/runtime/runtime.h"

Entity::Entity()
{
	transform = Transform();

	model = nullptr;

	Runtime::linkEntity(this);
}