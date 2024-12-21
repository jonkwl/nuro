#pragma once

#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <sstream>

#include "../core/ecs/registry_state.h"
#include "../core/ecs/composed.h"

namespace ECS {

	//
	// GLOBAL REGISTRY
	//

	inline entt::basic_registry<uint32_t> registry;

	//
	// ENTITY RELATED FUNCTIONS
	//

	uint32_t createEntity();
	void destroyEntity(uint32_t entity);
	bool verifyEntity(uint32_t entity);

	template<typename T, typename... Args>
	T& addComponent(uint32_t entity, Args&&... args) {
		return ECS::registry.emplace<T>(entity, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent(uint32_t entity) {
		return ECS::registry.get<T>(entity);
	}

	template<typename T>
	bool hasComponent(uint32_t entity) {
		return ECS::registry.any_of<T>(entity);
	}

	template<typename T>
	void removeComponent(uint32_t entity) {
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