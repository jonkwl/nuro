#include "entity.h"

#include "../core/rendering/core/mesh_renderer.h"

Entity::Entity() : transform(),
meshRenderer(MeshRenderer(this))
{
}