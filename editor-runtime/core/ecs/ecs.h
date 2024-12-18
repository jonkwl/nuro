#pragma once

#include <entt.hpp>
#include <tuple>
#include <optional>

#include "components.h"

namespace ECS {

	entt::registry& getRegistry();

	entt::entity createEntity();

	std::optional<std::tuple<TransformComponent&, CameraComponent&>> getActiveCamera();

}