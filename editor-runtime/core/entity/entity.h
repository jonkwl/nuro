#pragma once

#include "../core/transform/transform.h"
#include "../core/rendering/core/mesh_renderer.h"

class Entity
{
public:
	Entity();

	Transform transform;
	MeshRenderer meshRenderer;
};