#include "physics_context.h"

#include <utils/console.h>
#include <transform/transform.h>
#include <diagnostics/profiler.h>
#include <physics/utils/px_translator.h>

using namespace physx;

PhysicsContext::PhysicsContext() : allocator(),
errorCallback(),
foundation(nullptr),
physics(nullptr),
dispatcher(nullptr),
scene(nullptr),
pvd(nullptr),
bridge(physics, scene),
timeStep(1.0f / 60.0f),
gravity(PxVec3(0.0f, -9.81f, 0.0f)),
accumulatedTime(0.0f)
{
}

void PhysicsContext::create()
{
	// Create physx native instances
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
	dispatcher = PxDefaultCpuDispatcherCreate(2);

	// Create scene
	PxSceneDesc sceneDescription(physics->getTolerancesScale());
	sceneDescription.gravity = gravity;
	sceneDescription.cpuDispatcher = dispatcher;
	sceneDescription.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDescription);

	// Create pvd client
	PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	// Setup ecs listener
	bridge.setup();

	//
	// Register all observer events
	//

	ECS& ecs = ECS::main();

	ecs.reg().on_construct<BoxColliderComponent>().connect<&PhysicsBridge::constructBoxCollider>(bridge);
	ecs.reg().on_destroy<BoxColliderComponent>().connect<&PhysicsBridge::destroyBoxCollider>(bridge);

	ecs.reg().on_construct<SphereColliderComponent>().connect<&PhysicsBridge::constructSphereCollider>(bridge);
	ecs.reg().on_destroy<SphereColliderComponent>().connect<&PhysicsBridge::destroySphereCollider>(bridge);

	ecs.reg().on_construct<RigidbodyComponent>().connect<&PhysicsBridge::constructRigidbody>(bridge);
	ecs.reg().on_destroy<RigidbodyComponent>().connect<&PhysicsBridge::destroyRigidbody>(bridge);

}

void PhysicsContext::destroy()
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

void PhysicsContext::step(float delta)
{
	// Start profiler
	Profiler::start("physics");

	//
	// PHYSICS SIMULATION TIME STEP UPDATE
	//

	accumulatedTime += delta;
	while (accumulatedTime >= timeStep) {
		simulate(delta);
		accumulatedTime -= timeStep;
	}

	//
	// FRAME UPDATE
	// 

	// Sync transform components
	auto view = ECS::main().view<TransformComponent, RigidbodyComponent>();
	for (auto [entity, transform, rigidbody] : view.each()) {
		syncTransformComponent(delta, transform, rigidbody);
	}

	// Stop profiler
	Profiler::stop("physics");
}

void PhysicsContext::simulate(float delta)
{
	// Simulate physics
	scene->simulate(timeStep);
	scene->fetchResults(true);

	// Sync rigidbody components
	auto view = ECS::main().view<RigidbodyComponent>();
	for (auto [entity, rigidbody] : view.each()) {
		syncRigidbodyComponent(rigidbody);
	}
}

void PhysicsContext::syncRigidbodyComponent(RigidbodyComponent& rigidbody)
{
	PxRigidDynamic* actor = rigidbody.actor;

	PxVec3 linearVelocity = actor->getLinearVelocity();
	PxVec3 angularVelocity = actor->getAngularVelocity();

	// Flags
	rigidbody.sleeping = actor->isSleeping();
	rigidbody.moving = !(linearVelocity.isZero() && angularVelocity.isZero());

	// Velocity data
	rigidbody.velocity = PxTranslator::convert(linearVelocity);
	rigidbody.angularVelocity = PxTranslator::convert(angularVelocity);

	// Ttransform data
	PxTransform globalPose = actor->getGlobalPose();
	rigidbody.position = PxTranslator::convert(globalPose.p);
	rigidbody.rotation = PxTranslator::convert(globalPose.q);
}

void PhysicsContext::syncTransformComponent(float delta, TransformComponent& transform, RigidbodyComponent& rigidbody)
{
	// Handle kinematic rigidbodies: Set rigidbody actors global pose but dont change transform component
	if (rigidbody.kinematic) {
		rigidbody.actor->setGlobalPose(PxTransform(PxTranslator::convert(transform.position), PxTranslator::convert(transform.rotation)));
		return;
	}

	// Get new transform data
	glm::vec3 position = rigidbody.position;
	glm::quat rotation = rigidbody.rotation;

	// Interpolation
	/*float factor = delta / timeStep;
	switch (rigidbody.interpolation) {
	case RB_Interpolation::INTERPOLATE:
		position = interpolate(transform.position, position, factor);
		rotation = interpolate(transform.rotation, rotation, factor);
		break;
	case RB_Interpolation::EXTRAPOLATE:
		position = transform.position + rigidbody.velocity * delta;
		rotation = transform.rotation * glm::normalize(glm::quat(1.0f, rigidbody.angularVelocity * delta));
		break;
	}*/

	// Apply transform if rigidbody is moving
	if (rigidbody.moving) {
		Transform::setPosition(transform, position);
		Transform::setRotation(transform, rotation);
	}
}

glm::vec3 PhysicsContext::interpolate(glm::vec3 lastPosition, glm::vec3 position, float factor)
{
	return glm::mix(lastPosition, position, factor);
}

glm::quat PhysicsContext::interpolate(glm::quat lastRotation, glm::quat rotation, float factor)
{
	return glm::slerp(lastRotation, rotation, factor);
}