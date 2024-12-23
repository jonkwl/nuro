#pragma once

#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <sstream>
#include <tuple>

#include "../core/ecs/registry_state.hpp"
#include "../core/ecs/components.h"
#include "../core/ecs/composed.h"

namespace ECS {

	//
	// GLOBAL REGISTRY
	//

	inline entt::registry gRegistry;

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