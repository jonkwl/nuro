#pragma once

#include <tuple>
#include <sstream>
#include <cstdint>
#include <entt.hpp>
#include <optional>

#include "../src/core/ecs/components.h"

using namespace entt::literals;

using Entity = entt::entity;
using Registry = entt::registry;
using RenderQueue = std::vector<std::tuple<Entity, TransformComponent&, MeshRendererComponent&>>;
using Camera = std::tuple<TransformComponent&, CameraComponent&>;

namespace ECS {

	//
	// GLOBAL REGISTRY
	//

	inline Registry gRegistry;

	// Sets up global entity component system
	void setup();

	//
	// CORE FUNCTIONS
	//

	// Creates an entity
	std::tuple<Entity, TransformComponent&> createEntity(TransformComponent* parent = nullptr);

	// Returns the global render queue
	RenderQueue& getRenderQueue();

	// (Re-)Generates the global render queue
	void generateRenderQueue();

	//
	// HELPER FUNCTIONS
	//

	// Returns latest camera in register if existing
	std::optional<Camera> getLatestCamera();

}