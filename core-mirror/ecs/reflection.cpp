#include "reflection.h"

#include <string>
#include <entt.hpp>

#include "../src/core/ecs/components.h"

using namespace entt::literals;

namespace Reflection {

	struct ComponentMeta {
		std::string name;

		ComponentMeta(std::string name) :
			name(name)
		{};
	};

	void setup() {

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