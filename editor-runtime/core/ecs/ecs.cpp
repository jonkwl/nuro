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

	RegistryState captureState()
	{
		RegistryState state;
		entt::basic_snapshot{ registry }
			.get<uint32_t>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);

		return state;
	}

	void loadState(RegistryState& state)
	{
		registry.clear();

		entt::basic_snapshot_loader{ registry }
			.get<uint32_t>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);
	}

}