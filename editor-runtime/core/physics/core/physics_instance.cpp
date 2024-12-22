#include "physics_instance.h"

PhysicsInstance::PhysicsInstance() : allocator(),
errorCallback(),
foundation(nullptr),
physics(nullptr),
dispatcher(nullptr),
scene(nullptr),
defaultMaterial(nullptr),
pvd(nullptr),
timeStep(1.0f / 60.0f),
gravity(PxVec3(0.0f, -9.81f, 0.0f)),
accumulatedTime(0.0f)
{
}

void PhysicsInstance::create()
{
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);

	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);

	dispatcher = PxDefaultCpuDispatcherCreate(2);

	PxSceneDesc sceneDescription(physics->getTolerancesScale());
	sceneDescription.gravity = gravity;
	sceneDescription.cpuDispatcher = dispatcher;
	sceneDescription.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDescription);

	PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	defaultMaterial = physics->createMaterial(0.6f, 0.6f, 0.0f);
	defaultMaterial->setFrictionCombineMode(PxCombineMode::eAVERAGE);
	defaultMaterial->setRestitutionCombineMode(PxCombineMode::eAVERAGE);

	// tmp
	defaultMaterial->setRestitution(0.4f);

}

void PhysicsInstance::destroy()
{
	PX_RELEASE(scene);
	PX_RELEASE(dispatcher);
	PX_RELEASE(physics);
	if (pvd) {
		PxPvdTransport* transport = pvd->getTransport();
		PX_RELEASE(pvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(foundation);
}

void PhysicsInstance::step(float delta)
{
	accumulatedTime += delta;

	while (accumulatedTime >= timeStep) {

		scene->simulate(timeStep);
		scene->fetchResults(true);
		accumulatedTime -= timeStep;

	}
}

PxRigidStatic* PhysicsInstance::createStaticPlane(PxVec3 xyz, float distance) {
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(xyz.x, xyz.y, xyz.z, distance), *defaultMaterial);

	scene->addActor(*plane);

	return plane;
}

PxRigidDynamic* PhysicsInstance::createDynamicBox(PxVec3 position, PxQuat rotation, PxVec3 size)
{
	PxShape* shape = physics->createShape(PxBoxGeometry(size / 2), *defaultMaterial);
	PxRigidDynamic* body = physics->createRigidDynamic(PxTransform(position, rotation));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
	scene->addActor(*body);
	shape->release();
	return body;
}

void PhysicsInstance::createPyramidTest() {
	float halfExtent = 0.5f;
	physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *defaultMaterial);
	physx::PxU32 size = 30;
	physx::PxTransform t(physx::PxVec3(0));
	for (physx::PxU32 i = 0; i < size; i++) {
		for (physx::PxU32 j = 0; j < size - i; j++) {
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* body = physics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			scene->addActor(*body);
		}
	}
	shape->release();
}