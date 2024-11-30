#pragma once

#include "../src/transform/transform.h"
#include "../src/rendering/core/mesh_renderer.h"

struct Entity
{
public:
	Entity();

	Transform transform;
	MeshRenderer meshRenderer;
};