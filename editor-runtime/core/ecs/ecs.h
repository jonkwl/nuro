#pragma once

#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <sstream>

#include "../core/ecs/registry_state.h"
#include "../core/ecs/composed.h"

using entity = entt::entity;

namespace ECS {

	//
	// GLOBAL REGISTRY
	//

	inline entt::registry registry;

	//
	// ENTITY RELATED FUNCTIONS
	//

	entity createEntity();
	void destroyEntity(entity entity);
	bool verifyEntity(entity entity);

	template<typename T, typename... Args>
	T& addComponent(entity entity, Args&&... args) {
		return ECS::registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent(entity entity) {
		return ECS::registry.get<T>(entity);
	}

	template<typename T>
	bool hasComponent(entity entity) {
		return ECS::registry.any_of<T>(entity);
	}

	template<typename T>
	void removeComponent(entity entity) {
		ECS::registry.remove<T>(entity);
	}

	//
	// HELPER FUNCTIONS
	//

	std::optional<Camera> getLatestCamera(); // Returns latest camera in register if existing

	//
	// SERIALIZATION FUNCTIONS
	//

	RegistryState captureState(); // Captures current state of registry
	void loadState(RegistryState& state); // Loads given state into registry

}