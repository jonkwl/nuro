#pragma once

#include <glm.hpp>

#include "../src/transform/transform.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/material/imaterial.h"
#include "../src/rendering/model/model.h"

class Entity
{
public:
	Entity();

	Transform transform;

	Model* model;
};