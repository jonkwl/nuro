#include "rigidbody.h"

#include <PxPhysicsAPI.h>

using namespace physx;

namespace Rigidbody {

	void setResistance(RigidbodyComponent& rigidbody, float value)
	{
		rigidbody.actor->setLinearDamping(value);
	}

	void setAngularResistance(RigidbodyComponent& rigidbody, float value)
	{
		rigidbody.actor->setAngularDamping(value);
	}

	void setGravity(RigidbodyComponent& rigidbody, bool value)
	{
		rigidbody.actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value);
	}

	void setKinematic(RigidbodyComponent& rigidbody, bool value)
	{
		rigidbody.actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
	}

}