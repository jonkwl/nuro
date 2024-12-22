#include "physics_controller.h"

#include "../core/ecs/ecs_collection.h"

#include "../core/utils/log.h"

using namespace physx;

PhysicsController::PhysicsController() : allocator(),
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

void PhysicsController::create()
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

	// tmp
	exampleRigidbody = createDynamicBox(PxVec3(0.0f, 0.0f, 14.0f));
	createStaticPlane(PxVec3(0.0f, 1.0f, 0.0f), 6.0f);

}

void PhysicsController::destroy()
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

void PhysicsController::step(float delta)
{
	accumulatedTime += delta;
	while (accumulatedTime >= timeStep) {
		simulate();
		accumulatedTime -= timeStep;
	}
}

const physx::PxMaterial* PhysicsController::getDefaultMaterial() const
{
	return defaultMaterial;
}

physx::PxRigidActor* PhysicsController::createStaticPlane(PxVec3 xyz, float distance) {
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(xyz.x, xyz.y, xyz.z, distance), *defaultMaterial);

	scene->addActor(*plane);

	return plane;
}

physx::PxRigidActor* PhysicsController::createDynamicBox(PxVec3 position, PxQuat rotation, PxVec3 size)
{
	PxShape* shape = physics->createShape(PxBoxGeometry(size / 2), *defaultMaterial);
	PxRigidDynamic* body = physics->createRigidDynamic(PxTransform(position, rotation));
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
	scene->addActor(*body);
	shape->release();
	return body;
}

void PhysicsController::simulate()
{
	// Simulate physics
	scene->simulate(timeStep);
	scene->fetchResults(true);

	// Update rigidbody transform components
	auto view = ECS::registry.view<TransformComponent, RigidbodyComponent>();
	for (auto [entity, transform, rigidbody] : view.each()) {
		PxTransform globalPose = exampleRigidbody->getGlobalPose();
		transform.position = glm::vec3(globalPose.p.x, globalPose.p.y, globalPose.p.z);
		transform.rotation = glm::quat(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w);
	}
}