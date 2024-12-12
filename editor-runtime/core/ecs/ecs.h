#pragma once

#include <entt.hpp>

#include "components.h"

namespace ECS {

	entt::registry& getRegistry();

	entt::entity createEntity();

}