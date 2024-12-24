#pragma once

#include <cstdint>
#include <entt.hpp>
#include <optional>
#include <sstream>
#include <tuple>

#include "../core/ecs/registry_state.hpp"
#include "../core/ecs/components.h"
#include "../core/ecs/composed.h"

using RenderQueue = std::vector<std::tuple<entt::entity, TransformComponent&, MeshRendererComponent&>>;

namespace ECS {

	//
	// GLOBAL REGISTRY
	//

	inline entt::registry gRegistry;

	//
	// CORE FUNCTIONS
	//

	// Returns the global render queue
	RenderQueue& getRenderQueue();

	// (Re-)Generates the global render queue
	void generateRenderQueue();

	//
	// SERIALIZATION FUNCTIONS
	//

	// Captures current state of registry
	RegistryState captureState();

	// Loads given state into registry
	void loadState(RegistryState& state);

	//
	// HELPER FUNCTIONS
	//

	// Returns latest camera in register if existing
	std::optional<Camera> getLatestCamera();

}