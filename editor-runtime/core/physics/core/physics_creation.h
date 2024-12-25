#pragma once

#include <PxPhysicsAPI.h>
#include <glm.hpp>

#include "../core/ecs/components.h"

namespace PhysicsCreation
{

	physx::PxMaterial* createMaterial(physx::PxPhysics*& physics, float staticFriction = 0.6f, float dynamicFriction = 0.6, float restitution = 0.0f);

	physx::PxShape* createBoxCollider(physx::PxPhysics*& physics, const BoxColliderComponent& boxCollider);
	physx::PxShape* createSphereCollider(physx::PxPhysics*& physics, const SphereColliderComponent& sphereCollider);
	
	void attachCollider(physx::PxRigidDynamic*& actor, physx::PxShape*& shape);
	void destroyCollider(physx::PxShape*& shape);

	physx::PxRigidDynamic* createRigidbody(physx::PxPhysics*& physics, physx::PxScene*& scene, const TransformComponent& transform, RigidbodyComponent& rigidbody);
	void destroyRigidbody(physx::PxScene*& scene, physx::PxRigidDynamic*& actor);

};