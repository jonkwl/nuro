#include "reflection.h"

#include <entt.hpp>
#include "../core/ecs/components.h"

using namespace entt::literals;

namespace Reflection {

	void setup() {

		//
		// TRANSFORM COMPONENT
		//

		entt::meta<TransformComponent>()
			.type("Transform"_hs)
			.data<&TransformComponent::position>("Position"_hs)
			.prop("Requires"_hs, 125);
	}

}