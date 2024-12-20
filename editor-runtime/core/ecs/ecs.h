#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "../core/ecs/composed.h"

namespace ECS {

	// Global registry
	inline entt::registry registry;

	// Helper functions
	std::optional<Camera> getLatestCamera(); // Returns latest camera in register if existing

}