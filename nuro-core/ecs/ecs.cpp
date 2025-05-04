#include "ecs.h"

#include <random>
#include <algorithm>
#include <glm/glm.hpp>

#include <utils/console.h>
#include <audio/audio_context.h>
#include <transform/transform.h>

ECS::ECS() : registry(), idCounter(0), renderQueue()
{
	// Setup ecs component reflection
	ECSReflection::registerAll();

	// Register mesh renderer events
	registry.on_construct<MeshRendererComponent>().connect<&ECS::insertMeshRenderer>(this);
	registry.on_destroy<MeshRendererComponent>().connect<&ECS::purgeMeshRenderer>(this);

	// Register audio source events
	registry.on_construct<AudioSourceComponent>().connect<&AudioContext::constructAudioSource>();
	registry.on_destroy<AudioSourceComponent>().connect<&AudioContext::destroyAudioSource>();
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
	// Ensure parent is valid, create entity without parent otherwise
	if (!has<TransformComponent>(parent)) {
		Console::out::warning("ECS", "Tried to create entity '" + name + "' with invalid parent");
		return createEntity(name);
	}

	// Create new entity
	auto [entity, transform] = createEntity(name);

	// Fetch parent
	TransformComponent& parentTransform = get<TransformComponent>(parent);

	// Set parent
	setParent(entity, parent);

	// Return entity and transform component
	return std::tuple<Entity, TransformComponent&>(entity, transform);
}

void ECS::setParent(Entity entity, Entity parent)
{
	// Verify entity
	if (!has<TransformComponent>(entity)) {
		Console::out::warning("ECS", "Tried to modify parent of invalid entity");
		return;
	}

	// Verify parent
	if (!has<TransformComponent>(parent)) {
		Console::out::warning("ECS", "Tried to make invalid entity a parent");
		return;
	}

	// Remove current parent if any
	removeParent(entity);

	// Fetch transforms
	TransformComponent& transform = get<TransformComponent>(entity);
	TransformComponent& parentTransform = get<TransformComponent>(parent);

	// Add entity to parents children
	parentTransform.children.push_back(entity);

	// Update transform
	transform.parent = parent;
	transform.depth = parentTransform.depth + 1;
	transform.modified = true;
}

void ECS::removeParent(Entity entity)
{
	TransformComponent& transform = get<TransformComponent>(entity);

	// Return if entity doesn't have parent
	if (!Transform::hasParent(transform)) return;

	// Remove entity from children of parent
	// Inefficient, add e.g. binary search here later
	auto& children = Transform::fetchParent(transform).children;
	auto it = std::find(children.begin(), children.end(), entity);
	if (it != children.end()) {
		std::swap(*it, children.back());
		children.pop_back();
	}

	// Update transform
	transform.parent = entt::null;
	transform.depth = 0;
	transform.modified = true;
}

ECS& ECS::main()
{
	if (!entt::locator<ECS>::has_value()) 
		Console::out::error("ECS", "Fatal: No main ECS service was instantiated");

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