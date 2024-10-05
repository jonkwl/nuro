#include "entity.h"

#include "../engine/runtime/runtime.h"

Entity::Entity()
{
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);

	model = nullptr;

	Runtime::linkEntity(this);
}