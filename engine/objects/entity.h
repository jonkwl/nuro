#pragma once

#include <glm.hpp>

#include "../engine/rendering/texture/texture.h"

class Entity
{
public:
	Entity();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Texture* texture;
};