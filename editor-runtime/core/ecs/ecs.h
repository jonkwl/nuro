#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "components.h"

namespace ECS {

	entt::entity createEntity();

	void addTransformComponent(entt::entity& entity);
	void addMeshRendererComponent(entt::entity& entity);
	void addCameraComponent(entt::entity& entity);

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera();

}