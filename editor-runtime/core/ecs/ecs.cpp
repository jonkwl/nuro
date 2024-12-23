#include "ecs.h"

namespace ECS {

	std::tuple<entity, TransformComponent&> createEntity()
	{
		entity newEntity = gRegistry.create();
		TransformComponent& newTransform = addComponent<TransformComponent>(newEntity);
		return { newEntity, newTransform };
	}

	void destroyEntity(entity entity)
	{
		gRegistry.destroy(entity);
	}

	bool verifyEntity(entity entity)
	{
		return gRegistry.valid(entity);
	}

	std::optional<Camera> getLatestCamera() {
		auto group = gRegistry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			auto[transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
			return Camera(transform, camera);
		}
		return std::nullopt;
	}

	RegistryState captureState()
	{
		RegistryState state;
		entt::snapshot{ gRegistry }
			.get<entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);

		return state;
	}

	void loadState(RegistryState& state)
	{
		gRegistry.clear();

		entt::snapshot_loader{ gRegistry }
			.get<entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);
	}

}