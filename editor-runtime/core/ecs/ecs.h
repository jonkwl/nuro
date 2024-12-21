#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "../core/ecs/composed.h"

namespace ECS {

	// Global registry
	inline entt::basic_registry<uint32_t> registry;

	//
	// Entity related functions
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
	// Helper functions
	//

	std::optional<Camera> getLatestCamera(); // Returns latest camera in register if existing

}