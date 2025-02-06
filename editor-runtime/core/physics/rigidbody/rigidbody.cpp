#include "rigidbody.h"

#include <PxPhysicsAPI.h>

#include "../core/utils/console.h"
#include "../core/physics/utils/px_translator.h"

using namespace physx;

namespace Rigidbody {

	// Get current force mode
	PxForceMode::Enum _getBackendForceMode(RB_ForceMode mode) {
		switch (mode)
		{
		case RB_ForceMode::LINEAR:
			return PxForceMode::eFORCE;
			break;
		case RB_ForceMode::IMPULSE:
			return PxForceMode::eIMPULSE;
			break;
		case RB_ForceMode::VELOCITY_CHANGE:
			return PxForceMode::eVELOCITY_CHANGE;
			break;
		case RB_ForceMode::ACCELERATION:
			return PxForceMode::eACCELERATION;
			break;
		default:
			return PxForceMode::eFORCE;
			break;
		}
	}

	bool validate(RigidbodyComponent& rigidbody)
	{
		// Make sure rigidbody actor was created
		if (rigidbody.actor) {
			return true;
		}
		else {
			Console::out::warning("Rigidbody", "Invalid rigidbody was accessed!");
			return false;
		}
	}

	void setInterpolation(RigidbodyComponent& rigidbody, RB_Interpolation value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Set interpolation of rigidbody component
		rigidbody.interpolation = value;
	}

	void setCollisionDetection(RigidbodyComponent& rigidbody, RB_CollisionDetection value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// If actor is kinematic, always fall back to discrete collision detection
		if (rigidbody.kinematic) {
			value = RB_CollisionDetection::DISCRETE;
		}

		// Set actors collision detection according to given value
		switch (value) {
		case RB_CollisionDetection::DISCRETE:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
			break;
		case RB_CollisionDetection::CONTINUOUS:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
			break;
		case RB_CollisionDetection::CONTINUOUS_SPECULATIVE:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
			break;
		}

		// Sync component
		rigidbody.collisionDetection = value;
	}

	void setResistance(RigidbodyComponent& rigidbody, float value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Set actors linear damping according to value
		rigidbody.actor->setLinearDamping(value);

		// Sync component
		rigidbody.resistance = value;
	}

	void setAngularResistance(RigidbodyComponent& rigidbody, float value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Set actors angular damping according to value
		rigidbody.actor->setAngularDamping(value);

		// Sync component
		rigidbody.angularResistance = value;
	}

	void setGravity(RigidbodyComponent& rigidbody, bool value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Enable or disable actors gravity according to value
		rigidbody.actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value);

		// Sync component
		rigidbody.gravity = value;
	}

	void setKinematic(RigidbodyComponent& rigidbody, bool value)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Make sure actors collision detection is discrete
		setCollisionDetection(rigidbody, RB_CollisionDetection::DISCRETE);

		// Make actor kinematic or non-kinematic according to value
		rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);

		// Sync component
		rigidbody.kinematic = value;
	}

	void addForce(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Apply force to actor
		rigidbody.actor->addForce(PxTranslator::convert(value), _getBackendForceMode(mode));
	}

	void addTorque(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode)
	{
		// Validate rigidbody
		if (!validate(rigidbody)) return;

		// Apply torque to actor
		rigidbody.actor->addTorque(PxTranslator::convert(value), _getBackendForceMode(mode));
	}
}