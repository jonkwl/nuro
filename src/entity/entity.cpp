#include "entity.h"

#include "../src/runtime/runtime.h"
#include "../src/rendering/core/mesh_renderer.h"

Entity::Entity() : transform(),
meshRenderer(MeshRenderer(this))
{
	Runtime::linkEntity(this);
}