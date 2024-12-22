#pragma once

#include <PxPhysicsAPI.h>

class PhysicsController
{
public:
	PhysicsController();

	void create();
	void destroy();

	void step(float delta);

public:
	physx::PxRigidStatic* createStaticPlane(physx::PxVec3 xyz, float distance);
	physx::PxRigidDynamic* createDynamicBox(physx::PxVec3 position, physx::PxQuat rotation, physx::PxVec3 size);

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