#pragma once

#include "../core/ecs/components.h"

namespace Rigidbody
{

	// Sets collision detection of rigidbody
	void setCollisionDetection(RigidbodyComponent& rigidbody, RigidbodyComponent::CollisionDetection value);

	// Sets resistance coefficient of rigidbody
	void setResistance(RigidbodyComponent& rigidbody, float value);

	// Sets angular resistance coefficient of rigidbody
	void setAngularResistance(RigidbodyComponent& rigidbody, float value);

	// Sets if rigidbody will be affected by gravity
	void setGravity(RigidbodyComponent& rigidbody, bool value);

	// Sets if rigidbody should not be affected by physics
	void setKinematic(RigidbodyComponent& rigidbody, bool value);

};