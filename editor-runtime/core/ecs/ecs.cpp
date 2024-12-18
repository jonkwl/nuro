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

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera() {
		auto group = _registry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			return group.get<TransformComponent, CameraComponent>(entity);
		}
		return std::nullopt;
	}

}