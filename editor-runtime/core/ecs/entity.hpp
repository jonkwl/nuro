#include <entt.hpp>
#include <tuple>
#include <string>

#include "../core/ecs/ecs.h"
#include "../core/ecs/components.hpp"

static inline TransformComponent emptyTransform;

enum EmptyEntityFlag {
	EMPTY_ENTITY
};

struct Entity {

	// Construct entity with transform in global registry
	Entity() : registry(ECS::gRegistry), handle(registry.create()), transform(add<TransformComponent>()) {};
	
	// Construct empty entity with no instance in global registry
	Entity(EmptyEntityFlag flag) : registry(ECS::gRegistry), handle(), transform(emptyTransform) {};

	// Destroy entity in registry
	void destroy() {
		registry.destroy(handle);
	}

	// Returns if entity is valid in registry
	bool verify() {
		return registry.valid(handle);
	}

	// Adds a component of given component type with given arguments to the entity
	template<typename T, typename... Args>
	T& add(Args&&... args) {
		return ECS::gRegistry.emplace<T>(handle, std::forward<Args>(args)...);
	}

	// Returns component of given component type if attached to entity
	template<typename T>
	T& get() {
		return ECS::gRegistry.get<T>(handle);
	}

	// Returns true if entity has a component of given component type
	template<typename T>
	bool has() {
		return ECS::gRegistry.any_of<T>(handle);
	}

	// Removes component of given component type if attached to entity
	template<typename T>
	void remove() {
		ECS::gRegistry.remove<T>(handle);
	}

	// Compares entity class with ecs handle
	bool operator == (const entt::entity& other) const {
		return handle == other;
	}

	// Linked registry
	entt::registry& registry;

	// ECS handle
	entt::entity handle;

	// Entity transform
	TransformComponent& transform;

};