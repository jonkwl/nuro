#pragma once

#include "../src/transform/transform.h"

class MeshRenderer;

class Entity
{
public:
	Entity();

	Transform transform;
	MeshRenderer* meshRenderer;
};