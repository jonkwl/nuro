#include "entity.h"

#include "../src/runtime/runtime.h"

Entity::Entity()
{
	transform = Transform();

	meshRenderer = new MeshRenderer(this);

	Runtime::linkEntity(this);
}