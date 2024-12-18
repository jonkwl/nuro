#pragma once

#include "../core/ecs/components.h"
#include "../core/rendering/core/mesh_renderer.h"

class OldEntity
{
public:
	OldEntity();

	TransformComponent transform;
	MeshRenderer meshRenderer;
};