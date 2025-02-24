#pragma once

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace PxTranslator
{

	glm::vec2 convert(physx::PxVec2 input);
	glm::vec3 convert(physx::PxVec3 input);
	glm::vec4 convert(physx::PxVec4 input);

	physx::PxVec2 convert(glm::vec2 input);
	physx::PxVec3 convert(glm::vec3 input);
	physx::PxVec4 convert(glm::vec4 input);

	glm::quat convert(physx::PxQuat input);
	physx::PxQuat convert(glm::quat input);

};

