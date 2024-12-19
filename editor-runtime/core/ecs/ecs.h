#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "components.h"

#include "../core/rendering/model/mesh.h"

namespace ECS {

	entt::registry& getRegistry();

	entt::entity createEntity();

	void addTransformComponent(entt::entity& entity);
	void addMeshRendererComponent(entt::entity& entity, Mesh& mesh);
	void addCameraComponent(entt::entity& entity);

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera();

}