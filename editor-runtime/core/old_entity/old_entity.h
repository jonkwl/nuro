#pragma once

#include "../core/transform/transform.h"
#include "../core/rendering/core/mesh_renderer.h"

class OldEntity
{
public:
	OldEntity();

	Transform transform;
	MeshRenderer meshRenderer;
};