#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

class PhysicsInstance
{
public:
	PhysicsInstance();

	void create();
	void destroy();

	void step(float delta);

public:
	PxRigidStatic* createStaticPlane(PxVec3 xyz, float distance);
	PxRigidDynamic* createDynamicBox(PxVec3 position, PxQuat rotation, PxVec3 size);
	void createPyramidTest();

private:
	PxDefaultAllocator allocator;
	PxDefaultErrorCallback errorCallback;
	PxFoundation* foundation;
	PxPhysics* physics;
	PxDefaultCpuDispatcher* dispatcher;
	PxScene* scene;
	PxPvd* pvd;
	PxMaterial* defaultMaterial;

private:
	const PxReal timeStep;
	const PxVec3 gravity;

private:
	float accumulatedTime;

};