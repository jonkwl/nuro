#include "entity.h"

Entity::Entity() : handle()
{
}

Entity::Entity(entt::entity entity) : handle(entity)
{
}

Entity Entity::create()
{
	return Entity(ECS::registry.create());
}

void Entity::destroy()
{
	ECS::registry.destroy(handle);
}

bool Entity::isValid()
{
	return ECS::registry.valid(handle);
}
