#include "ecs.h"

namespace ECS {

	entt::registry _registry;

	entt::registry& getRegistry()
	{
		return _registry;
	}

	entt::entity createEntity()
	{
		entt::entity entity = _registry.create();
		_registry.emplace<TransformComponent>(entity);
		_registry.emplace<MeshRendererComponent>(entity);
		return entity;
	}

}