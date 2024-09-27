#pragma once

#include <glm.hpp>

#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/material/imaterial.h"
#include "../engine/rendering/model/model.h"

class Entity
{
public:
	Entity();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Model* model;
};