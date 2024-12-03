#pragma once

// This is a temporary namespace serving as a global pool for various resources enabling the development of certain features to be more efficient

#include <glm.hpp>

#include "../core/entity/entity.h"

namespace TmpContext
{

	inline Entity* selectedEntity = nullptr;
	inline glm::mat4 view = glm::mat4(1.0f);
	inline glm::mat4 projection = glm::mat4(1.0f);

};