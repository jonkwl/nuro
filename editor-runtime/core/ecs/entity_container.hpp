#pragma once

#define G_REGISTRY ECS::gRegistry

#include <entt.hpp>
#include <tuple>
#include <string>

#include "../core/ecs/ecs.h"
#include "../core/ecs/components.h"

struct EntityContainer {

	// Construct entity by root (non-explicit)
	EntityContainer(entt::entity root) : root(root), transform(G_REGISTRY.get<TransformComponent>(root)), registry(G_REGISTRY) {};

	// Construct entity container with data tuple (non-explicit)
	EntityContainer(std::tuple<entt::entity, TransformComponent&> data) : root(std::get<0>(data)), transform(std::get<1>(data)), registry(G_REGISTRY) {};

	// entt::entity root
	entt::entity root;
	
	// entt::entity transform
	TransformComponent& transform;

	// entt::entity registry
	entt::registry& registry;

	// Returns if entity is valid in registry
	bool verify() {
		return registry.valid(root);
	}

	// Adds a component of given component type with given arguments to the entity
	template<typename T, typename... Args>
	T& add(Args&&... args) {
		return registry.emplace<T>(root, std::forward<Args>(args)...);
	}

	// Returns component of given component type if attached to entity
	template<typename T>
	T& get() {
		return registry.get<T>(root);
	}

	// Returns true if entity has a component of given component type
	template<typename T>
	bool has() {
		return registry.any_of<T>(root);
	}

	// Removes component of given component type if attached to entity
	template<typename T>
	void remove() {
		registry.remove<T>(root);
	}

	// Compares entity class with ecs root
	bool operator == (const entt::entity& other) const {
		return root == other;
	}

};