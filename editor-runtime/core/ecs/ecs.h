#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "../core/ecs/components.h"
#include "../core/ecs/composed.h"
#include "../core/rendering/model/mesh.h"

namespace ECS {

	entt::registry& getRegistry();

	entt::entity createEntity();

	void addTransformComponent(entt::entity& entity);
	void addMeshRendererComponent(entt::entity& entity, Mesh& mesh);
	void addCameraComponent(entt::entity& entity);
	void addVelocityComponent(entt::entity& entity);

	std::optional<Camera> getActiveCamera();

}