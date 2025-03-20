#pragma once

#include <glm/glm.hpp>

#include <ecs/components.h>
#include <physics/rigidbody/rigidbody_enums.h>

namespace Rigidbody
{

	// Checks if a rigidbody component is associated with a valid rigidbody
	bool validate(RigidbodyComponent& rigidbody);

	// Sets interpolation of rigidbody
	void setInterpolation(RigidbodyComponent& rigidbody, RB_Interpolation interpolation);

	// Sets collision detection of rigidbody
	void setCollisionDetection(RigidbodyComponent& rigidbody, RB_CollisionDetection collisionDetection);

	// Sets resistance coefficient of rigidbody
	void setResistance(RigidbodyComponent& rigidbody, float resistance);

	// Sets angular resistance coefficient of rigidbody
	void setAngularResistance(RigidbodyComponent& rigidbody, float angularResistance);

	// Sets if rigidbody will be affected by gravity
	void setGravity(RigidbodyComponent& rigidbody, bool gravity);

	// Sets if rigidbody should not be affected by physics
	void setKinematic(RigidbodyComponent& rigidbody, bool kinematic);

	// Add force to rigidbody
	void addForce(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode = RB_ForceMode::LINEAR);

	// Add torque to rigidbody
	void addTorque(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode);

};