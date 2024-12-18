#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "components.h"

namespace ECS {

	entt::entity createEntity();

	void addCameraComponent(entt::entity& entity);

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera();

}