#pragma once

#include <tuple>
#include <sstream>
#include <cstdint>
#include <entt/entt.hpp>
#include <optional>

#include <ecs/components.h>

using namespace entt::literals;

using Entity = entt::entity;
using Registry = entt::registry;
using TransformQueue = std::vector<TransformComponent*>;
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
	std::tuple<Entity, TransformComponent&> createEntity(std::string name);
	std::tuple<Entity, TransformComponent&> createEntity(std::string name, Entity parent);

	// Returns the global transform queue
	const TransformQueue& getTransformQueue();

	// Returns the global render queue
	const RenderQueue& getRenderQueue();

	//
	// HELPER FUNCTIONS
	//

	// Returns latest camera in register if existing
	std::optional<Camera> getLatestCamera();

}