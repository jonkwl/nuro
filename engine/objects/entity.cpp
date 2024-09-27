#include "entity.h"

Entity::Entity()
{
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);

	material = nullptr;

	mesh = nullptr;
}