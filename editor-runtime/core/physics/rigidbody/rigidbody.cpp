#include "rigidbody.h"

#include <PxPhysicsAPI.h>

using namespace physx;

namespace Rigidbody {

	void setCollisionDetection(RigidbodyComponent& rigidbody, RigidbodyComponent::CollisionDetection value)
	{
		switch (value) {
		case RigidbodyComponent::CollisionDetection::DISCRETE:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
			break;
		case RigidbodyComponent::CollisionDetection::CONTINUOUS:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, false);
			break;
		case RigidbodyComponent::CollisionDetection::CONTINUOUS_SPECULATIVE:
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, false);
			rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, true);
			break;
		}
		rigidbody.collisionDetection = value;
	}

	void setResistance(RigidbodyComponent& rigidbody, float value)
	{
		rigidbody.actor->setLinearDamping(value);
		rigidbody.resistance = value;
	}

	void setAngularResistance(RigidbodyComponent& rigidbody, float value)
	{
		rigidbody.actor->setAngularDamping(value);
		rigidbody.angularResistance = value;
	}

	void setGravity(RigidbodyComponent& rigidbody, bool value)
	{
		rigidbody.actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value);
		rigidbody.gravity = value;
	}

	void setKinematic(RigidbodyComponent& rigidbody, bool value)
	{
		rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
		rigidbody.kinematic = value;
	}

}