#include "ecs_reflection.h"

#include <string>
#include <entt/entt.hpp>

#include <ecs/components.h>

using namespace entt::literals;

namespace ECSReflection {

	void registerAll() {

		//
		// TRANSFORM COMPONENT 
		//
		
		entt::meta<TransformComponent>()
			.type("Transform"_hs)

			.data<&TransformComponent::position>("Position"_hs)
			.data<&TransformComponent::rotation>("Rotation"_hs)
			.data<&TransformComponent::scale>("Scale"_hs);

		// ... etc. add components for serialization
	}
	       
}