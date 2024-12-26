#pragma once

#include <PxPhysicsAPI.h>
#include <glm.hpp>

namespace PhysicsFactory
{

	//
	// MATERIALS
	//

	physx::PxMaterial* createMaterial(physx::PxPhysics*& physics, float staticFriction = 0.6f, float dynamicFriction = 0.6, float restitution = 0.0f);

	//
	// SHAPES
	//

	physx::PxShape* createBoxShape(physx::PxPhysics*& physics, physx::PxMaterial*& material, const glm::vec3& size);
	physx::PxShape* createSphereShape(physx::PxPhysics*& physics, physx::PxMaterial*& material, float radius);


	//
	// RIGIDBODIES
	//

	physx::PxRigidDynamic* createDynamicRigidbody(physx::PxPhysics*& physics, physx::PxScene*& scene, const glm::vec3& position, const glm::quat& rotation);

};