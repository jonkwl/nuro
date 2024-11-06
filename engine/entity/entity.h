#pragma once

#include <glm.hpp>

#include "../engine/transform/transform.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/material/imaterial.h"
#include "../engine/rendering/model/model.h"

class Entity
{
public:
	Entity();

	Transform transform;

	Model* model;
};