#include "ecs.h"

#include <random>
#include <glm/glm.hpp>
#include <algorithm>

#include <utils/console.h>
#include <ecs/reflection.h>
#include <transform/transform.h>

ECS::ECS() : registry(), idCounter(0), renderQueue()
{
	// Setup component reflection
	Reflection::setup();

	// Register render queue updates
	registry.on_construct<MeshRendererComponent>().connect<&ECS::insertMeshRenderer>(this);
	registry.on_destroy<MeshRendererComponent>().connect<&ECS::purgeMeshRenderer>(this);
}

std::tuple<Entity, TransformComponent&> ECS::createEntity(std::string name)
{
	// Create new entity and emplace transform component
	Entity entity = registry.create();
	TransformComponent& transform = add<TransformComponent>(entity);

	// Set transform data
	transform.id = getId();
	transform.name = name;

	// Return entity and transform component
	return std::tuple<Entity, TransformComponent&>(entity, transform);
}

std::tuple<Entity, TransformComponent&> ECS::createEntity(std::string name, Entity parent)
{
	// Create new entity
	auto [entity, transform] = createEntity(name);

	// Link parent to transform
	transform.parent = parent;

	// Fetch parent
	TransformComponent& parentTransform = Transform::fetchParent(transform);

	// Add to parents children
	parentTransform.children.push_back(entity);

	// Update depth
	transform.depth = parentTransform.depth + 1;

	// Return entity and transform component
	return std::tuple<Entity, TransformComponent&>(entity, transform);
}

const RenderQueue& ECS::getRenderQueue()
{
	return renderQueue;
}

std::optional<Camera> ECS::getActiveCamera() {
	auto group = registry.group<TransformComponent>(entt::get<CameraComponent>);
	for (auto entity : group) {
		auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

		if (camera.enabled) return Camera(transform, camera);
		else return std::nullopt;
	}
	return std::nullopt;
}

ECS& ECS::main()
{
	if (!entt::locator<ECS>::has_value()) 
		Console::out::error("ECS", "Fatal: No main ECS service was instantiated!");

	return entt::locator<ECS>::value();
}

uint32_t ECS::getId() {
	idCounter++;
	return idCounter;
}

void ECS::insertMeshRenderer(Entity target) {

	//
	// NON-OPTIMAL BOILERPLATE CODE!
	// Right now, this code is just regenerating the render queue using all mesh renderer components
	// This is sub-optimal, it should just be inserted into the render queue at the right place
	//

	// Fill target queue
	std::vector<Entity> targetQueue;

	view<MeshRendererComponent>().each([&](auto entity, const auto&) {
		targetQueue.push_back(entity);
		});

	// Sort targets by shader and material
	std::sort(targetQueue.begin(), targetQueue.end(), [&](auto lhsEntity, auto rhsEntity) {
		MeshRendererComponent& lhs = get<MeshRendererComponent>(lhsEntity);
		auto lhsShaderId = lhs.material ? lhs.material->getShaderId() : -1;
		auto lhsMaterialId = lhs.material ? lhs.material->getId() : -1;

		MeshRendererComponent& rhs = get<MeshRendererComponent>(rhsEntity);
		auto rhsShaderId = rhs.material ? rhs.material->getShaderId() : -1;
		auto rhsMaterialId = rhs.material ? rhs.material->getId() : -1;

		return std::tie(lhsShaderId, lhsMaterialId) < std::tie(rhsShaderId, rhsMaterialId);
		});

	// Fill render queue
	renderQueue.clear();

	for (auto entity : targetQueue) {
		renderQueue.emplace_back(entity, get<TransformComponent>(entity), get<MeshRendererComponent>(entity));
	}

}

void ECS::purgeMeshRenderer(Entity target) {

	//
	// NON-OPTIMAL BOILERPLATE CODE!
	// Right now, this code is just regenerating the render queue using all mesh renderer components but skips the provided target entity
	// This is sub-optimal, it should just be removed from the render queue
	//

	// Fill target queue
	std::vector<Entity> targetQueue;

	view<MeshRendererComponent>().each([&](auto entity, const auto&) {
		if (entity != target) {
			targetQueue.push_back(entity);
		}
		});

	// Sort targets by shader and material
	std::sort(targetQueue.begin(), targetQueue.end(), [&](auto lhsEntity, auto rhsEntity) {
		MeshRendererComponent& lhs = get<MeshRendererComponent>(lhsEntity);
		auto lhsShaderId = lhs.material ? lhs.material->getShaderId() : -1;
		auto lhsMaterialId = lhs.material ? lhs.material->getId() : -1;

		MeshRendererComponent& rhs = get<MeshRendererComponent>(rhsEntity);
		auto rhsShaderId = rhs.material ? rhs.material->getShaderId() : -1;
		auto rhsMaterialId = rhs.material ? rhs.material->getId() : -1;

		return std::tie(lhsShaderId, lhsMaterialId) < std::tie(rhsShaderId, rhsMaterialId);
		});

	// Fill render queue
	renderQueue.clear();

	for (auto entity : targetQueue) {
		renderQueue.emplace_back(entity, get<TransformComponent>(entity), get<MeshRendererComponent>(entity));
	}

}