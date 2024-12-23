#pragma once

#include <PxPhysicsAPI.h>

class PhysicsController
{
public:
	PhysicsController();

	void create(); // Create physics
	void destroy(); // Destroy physics

	void step(float delta); // Steps physics and performs simulation iterations

	const physx::PxMaterial* getDefaultMaterial() const;

public:
	physx::PxRigidActor* createStaticPlane(physx::PxVec3 xyz, float distance); // Creates a static rigidbody with a plane collider
	physx::PxRigidActor* createDynamicBox(physx::PxVec3 position, physx::PxQuat rotation = physx::PxIdentity, physx::PxVec3 size = physx::PxVec3(1.0f)); // Creates a dynamic rigidbody with a box collider

	physx::PxRigidActor* tmpGetExampleRigidbody() const;

private:
	void simulate(); // Simulates physics iteration

	// tmp
	physx::PxRigidActor* exampleRigidbody = nullptr;

private:
	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
	physx::PxFoundation* foundation;
	physx::PxPhysics* physics;
	physx::PxDefaultCpuDispatcher* dispatcher;
	physx::PxScene* scene;
	physx::PxPvd* pvd;
	physx::PxMaterial* defaultMaterial;

private:
	const physx::PxReal timeStep;
	const physx::PxVec3 gravity;

private:
	float accumulatedTime;

};