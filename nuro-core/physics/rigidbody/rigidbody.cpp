#include "rigidbody.h"

#include <PxPhysicsAPI.h>

#include <utils/console.h>
#include <physics/utils/px_translator.h>

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
		if (rigidbody.actor) return true;
		else {
			Console::out::warning("Rigidbody", "Invalid rigidbody was accessed!");
			return false;
		}
	}

	void setInterpolation(RigidbodyComponent& rigidbody, RB_Interpolation interpolation)
	{
		if (!validate(rigidbody)) return;
		rigidbody.interpolation = interpolation;
	}

	void setCollisionDetection(RigidbodyComponent& rigidbody, RB_CollisionDetection collisionDetection)
	{
		if (!validate(rigidbody)) return;

		// If actor is kinematic, always fall back to discrete collision detection
		if (rigidbody.kinematic) collisionDetection = RB_CollisionDetection::DISCRETE;

		// Update actors collision detection
		switch (collisionDetection) {
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

		rigidbody.collisionDetection = collisionDetection;
	}

	void setResistance(RigidbodyComponent& rigidbody, float resistance)
	{
		if (!validate(rigidbody)) return;
		rigidbody.actor->setLinearDamping(resistance);
		rigidbody.resistance = resistance;
	}

	void setAngularResistance(RigidbodyComponent& rigidbody, float angularResistance)
	{
		if (!validate(rigidbody)) return;
		rigidbody.actor->setAngularDamping(angularResistance);
		rigidbody.angularResistance = angularResistance;
	}

	void setGravity(RigidbodyComponent& rigidbody, bool gravity)
	{
		if (!validate(rigidbody)) return;
		rigidbody.actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !gravity);
		rigidbody.gravity = gravity;
	}

	void setKinematic(RigidbodyComponent& rigidbody, bool kinematic)
	{
		if (!validate(rigidbody)) return;

		// Make sure actors collision detection is discrete
		setCollisionDetection(rigidbody, RB_CollisionDetection::DISCRETE);

		rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
		rigidbody.kinematic = kinematic;
	}

	void addForce(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode)
	{
		if (!validate(rigidbody)) return;
		rigidbody.actor->addForce(PxTranslator::convert(value), _getBackendForceMode(mode));
	}

	void addTorque(RigidbodyComponent& rigidbody, glm::vec3 value, RB_ForceMode mode)
	{
		if (!validate(rigidbody)) return;
		rigidbody.actor->addTorque(PxTranslator::convert(value), _getBackendForceMode(mode));
	}
}