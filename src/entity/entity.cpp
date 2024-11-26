#include "entity.h"

#include "../src/runtime/runtime.h"
#include "../src/rendering/core/mesh_renderer.h"

Entity::Entity()
{
	transform = Transform();
	meshRenderer = new MeshRenderer(this);

	Runtime::linkEntity(this);
}