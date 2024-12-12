#include "old_entity.h"

#include "../core/rendering/core/mesh_renderer.h"

OldEntity::OldEntity() : transform(),
meshRenderer(MeshRenderer(this))
{
}