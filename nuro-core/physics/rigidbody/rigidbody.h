#pragma once

#include <glm/glm.hpp>

#include <ecs/components.h>
#include <physics/rigidbody/rigidbody_enums.h>

namespace Rigidbody
{

	// Checks if a rigidbody components is associated with a valid rigidbody
	bool validate(RigidbodyComponent& rigidbody);

	// Sets interpolation of rigidbody
	void setInterpolation(RigidbodyComponent& rigidbody, RB_Interpolation value);

	// Sets collision detection of rigidbody
	void setCollisionDetection(RigidbodyComponent& rigidbody, RB_CollisionDetection value);

	// Sets resistance coefficient of rigidbody
	void setResistance(RigidbodyComponent& rigidbody, float value);

	// Sets angular resistance coefficient of rigidbody
	void setAngularResistance(RigidbodyComponent& rigidbody, float value);

	// Sets if rigidbody will be affected by gravity
	void setGravity(RigidbodyComponent& rigidbody, bool value);

	// Sets if rigidbody should not be affected by physics
	void setKinematic(RigidbodyComponent& rigidbody, bool value);

	// Add force
	void addForce(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode = RB_ForceMode::LINEAR);

};