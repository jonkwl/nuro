#include "ecs.h"

namespace ECS {

	entt::entity createEntity()
	{
		return registry.create();
	}

	std::optional<Camera> getLatestCamera() {
		auto group = registry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			auto[transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
			return Camera(transform, camera);
		}
		return std::nullopt;
	}

}