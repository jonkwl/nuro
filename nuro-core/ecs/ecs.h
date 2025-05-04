#pragma once

#include <tuple>
#include <memory>
#include <sstream>
#include <cstdint>
#include <optional>
#include <entt/entt.hpp>

#include <utils/console.h>
#include <ecs/components.h>
#include <ecs/ecs_reflection.h>

using namespace entt::literals;

using Entity = entt::entity;
using Registry = entt::registry;
using RenderQueue = std::vector<std::tuple<Entity, TransformComponent&, MeshRendererComponent&>>;
using Camera = std::tuple<TransformComponent&, CameraComponent&>;

class ECS {
public:
	ECS();

	// Returns the render queue
	const RenderQueue& getRenderQueue();

	// Returns the camera currently rendering
	std::optional<Camera> getActiveCamera();

	// Creates a new entity
	std::tuple<Entity, TransformComponent&> createEntity(std::string name);

	// Creates a new entity with a parent
	std::tuple<Entity, TransformComponent&> createEntity(std::string name, Entity parent);

	// Sets the parent of an entity
	void setParent(Entity entity, Entity parent);

	// Removes the parent of an entity if it has one
	void removeParent(Entity entity);

public:
	// Returns a reference to the registry; use with caution, prefer wrapper methods!
	Registry& reg() {
		return registry;
	}

	// Creates a view
	template<typename... Components>
	auto view() {
		return registry.view<Components...>();
	}

	// Returns if entity is valid
	bool verify(Entity entity) const {
		return registry.valid(entity);
	}

	// Returns if entity has a component of given type
	template<typename T>
	bool has(Entity entity) const {
		return registry.any_of<T>(entity);
	}

	// Returns if a component of given type can be added to entity
	template<typename T>
	bool canAdd(Entity entity) const {
		return !registry.any_of<T>(entity);
	}

	// Adds a component of given type to entity (check canAdd() first!)
	template<typename T, typename... Args>
	T& add(Entity entity, Args&&... args) {
		return registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	// Returns component of given type attached to entity (check has() first!)
	template<typename T>
	T& get(Entity entity) {
		return registry.get<T>(entity);
	}

	// Removes component of given type from entity
	template<typename T>
	void remove(Entity entity) {
		registry.remove<T>(entity);
	}

public:
	// Locates the main ECS service
	static ECS& main();

private:
	Registry registry;
	uint32_t idCounter;
	RenderQueue renderQueue;

	// Returns a unique id
	uint32_t getId();

	// Inserts the target entity and its mesh renderer component to the render queue
	void insertMeshRenderer(Entity target);

	// Purges the target entity and its mesh renderer component from the render queue
	void purgeMeshRenderer(Entity target);
};