#include "ecs.h"

#include <random>
#include <algorithm>
#include "../core/utils/log.h"

namespace ECS {

	RenderQueue gRenderQueue;

	void generateRenderQueue()
	{
		// Fill unsorted queue
		std::vector<entt::entity> unsortedQueue;
		gRegistry.view<MeshRendererComponent>().each([&](auto entity, const auto&) { unsortedQueue.push_back(entity); });

		// Sort by shader and material
		/*std::sort(unsortedQueue.begin(), unsortedQueue.end(), [&](auto lhsEntity, auto rhsEntity) {
			MeshRendererComponent& lhs = gRegistry.get<MeshRendererComponent>(lhsEntity);
			MeshRendererComponent& rhs = gRegistry.get<MeshRendererComponent>(rhsEntity);
			return std::tie(lhs.material->getShaderId(), lhs.material->getId()) < std::tie(rhs.material->getShaderId(), rhs.material->getId());
			}); */

		// Fill render queue
		gRenderQueue.clear();
		for (auto entity : unsortedQueue) {
			gRenderQueue.emplace_back(entity, gRegistry.get<TransformComponent>(entity), gRegistry.get<MeshRendererComponent>(entity));
		}
	}

	std::tuple<entt::entity, TransformComponent&> createEntity()
	{
		entt::entity entity = gRegistry.create();
		TransformComponent& transform = gRegistry.emplace<TransformComponent>(entity);
		return std::tuple<entt::entity, TransformComponent&>(entity, transform);
	}

	RenderQueue& getRenderQueue()
	{
		return gRenderQueue;
	}

	RegistryState captureState()
	{
		RegistryState state;
		entt::snapshot{ gRegistry }
			.get<entt::entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);

		return state;
	}

	void loadState(RegistryState& state)
	{
		gRegistry.clear();

		entt::snapshot_loader{ gRegistry }
			.get<entt::entity>(state)
			.get<TransformComponent>(state)
			.get<MeshRendererComponent>(state)
			.get<CameraComponent>(state);
	}

	std::optional<Camera> getLatestCamera() {
		auto group = gRegistry.group<TransformComponent>(entt::get<CameraComponent>);
		for (auto entity : group) {
			auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
			return Camera(transform, camera);
		}
		return std::nullopt;
	}

}