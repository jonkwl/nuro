#include "entity.h"

#include "../engine/runtime/runtime.h"

Entity::Entity()
{
	transform = Transform();

	model = nullptr;

	Runtime::linkEntity(this);
}