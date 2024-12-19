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

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera() {
		auto group = _registry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			return group.get<TransformComponent, CameraComponent>(entity);
		}
		return std::nullopt;
	}

}