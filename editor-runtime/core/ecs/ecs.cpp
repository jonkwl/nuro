#include "ecs.h"

namespace ECS {

	entity createEntity()
	{
		return registry.create();
	}

	void destroyEntity(entity entity)
	{
		registry.destroy(entity);
	}

	bool verifyEntity(entity entity)
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

	RegistryState captureState()
	{
		RegistryState state;
		entt::snapshot{ registry }
			.get<entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);

		return state;
	}

	void loadState(RegistryState& state)
	{
		registry.clear();

		entt::snapshot_loader{ registry }
			.get<entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);
	}

}