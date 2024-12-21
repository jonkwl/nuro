#include "ecs.h"

namespace ECS {

	uint32_t createEntity()
	{
		return registry.create();
	}

	void destroyEntity(uint32_t entity)
	{
		registry.destroy(entity);
	}

	bool verifyEntity(uint32_t entity)
	{
		return registry.valid(entity);
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