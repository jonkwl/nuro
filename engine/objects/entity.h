#pragma once

#include <glm.hpp>

class Entity
{
public:
	Entity();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};