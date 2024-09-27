#pragma once

#include <glm.hpp>

#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/material/imaterial.h"

class Entity
{
public:
	Entity();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	IMaterial* material;
};