#include "ecs.h"

namespace ECS {

	entt::registry _registry;

	entt::registry& getRegistry()
	{
		return _registry;
	}

	entt::entity createEntity()
	{
		return _registry.create();
	}

	void addTransformComponent(entt::entity& entity)
	{
		_registry.emplace<TransformComponent>(entity);
	}

	void addMeshRendererComponent(entt::entity& entity, Mesh& mesh)
	{
		_registry.emplace<MeshRendererComponent>(entity, mesh);
	}

	void addCameraComponent(entt::entity& entity)
	{
		_registry.emplace<CameraComponent>(entity);
	}

	void addVelocityComponent(entt::entity& entity)
	{
		_registry.emplace<VelocityComponent>(entity);
	}

	std::optional<Camera> getActiveCamera() {
		auto group = _registry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			auto[transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
			return Camera(transform, camera);
		}
		return std::nullopt;
	}

}