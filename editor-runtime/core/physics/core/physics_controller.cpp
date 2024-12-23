#include "physics_controller.h"

#include "../core/utils/log.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/physics/utils/px_translator.h"

using namespace physx;

PhysicsController::PhysicsController() : allocator(),
errorCallback(),
foundation(nullptr),
physics(nullptr),
dispatcher(nullptr),
scene(nullptr),
pvd(nullptr),
defaultMaterial(nullptr),
observer(physics, scene, defaultMaterial),
timeStep(1.0f / 60.0f),
gravity(PxVec3(0.0f, -9.81f, 0.0f)),
accumulatedTime(0.0f)
{
}

void PhysicsController::create()
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

	// Create default material
	defaultMaterial = physics->createMaterial(0.6f, 0.6f, 0.0f);
	defaultMaterial->setFrictionCombineMode(PxCombineMode::eAVERAGE);
	defaultMaterial->setRestitutionCombineMode(PxCombineMode::eAVERAGE);

	// Register observer events
	ECS::gRegistry.on_construct<BoxColliderComponent>().connect<&PhysicsObserver::constructBoxCollider>(observer);
	ECS::gRegistry.on_destroy<BoxColliderComponent>().disconnect<&PhysicsObserver::destroyBoxCollider>(observer);

	ECS::gRegistry.on_construct<RigidbodyComponent>().connect<&PhysicsObserver::constructRigidbody>(observer);
	ECS::gRegistry.on_destroy<RigidbodyComponent>().disconnect<&PhysicsObserver::destroyRigidbody>(observer);

	// tmp
	defaultMaterial->setRestitution(0.4f);

	// tmp
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(0.0f, 1.0f, 0.0f, 8.0f), *defaultMaterial);
	scene->addActor(*plane);

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

	// Update transform components
	auto view = ECS::gRegistry.view<TransformComponent, RigidbodyComponent>();
	for (auto [entity, transform, rigidbody] : view.each()) {
		syncTransformComponent(delta, transform, rigidbody);
	}
}

const physx::PxMaterial* PhysicsController::getDefaultMaterial() const
{
	return defaultMaterial;
}

void PhysicsController::simulate(float delta)
{
	// Simulate physics
	scene->simulate(timeStep);
	scene->fetchResults(true);

	// Update rigidbody components
	auto view = ECS::gRegistry.view<RigidbodyComponent>();
	for (auto [entity, rigidbody] : view.each()) {
		syncRigidbodyComponent(rigidbody);
	}
}

void PhysicsController::syncRigidbodyComponent(RigidbodyComponent& rigidbody)
{
	// Get new transform data
	PxTransform globalPose = rigidbody.actor->getGlobalPose();
	rigidbody.position = PxTranslator::convert(globalPose.p);
	rigidbody.rotation = PxTranslator::convert(globalPose.q);
}

void PhysicsController::syncTransformComponent(float delta, TransformComponent& transform, RigidbodyComponent& rigidbody)
{
	// Get new transform data
	glm::vec3 position = rigidbody.position;
	glm::quat rotation = rigidbody.rotation;

	// Interpolation
	if (rigidbody.interpolation == RigidbodyComponent::Interpolation::INTERPOLATE) {
		float factor = delta / timeStep;

		// Get current transform data without update
		glm::vec3 lastPosition = transform.position;
		glm::quat lastRotation = transform.rotation;

		position = interpolate(lastPosition, position, factor);
		rotation = interpolate(lastRotation, rotation, factor);
	}

	// Apply to transform
	transform.position = position;
	transform.rotation = rotation;
}

glm::vec3 PhysicsController::interpolate(glm::vec3 lastPosition, glm::vec3 position, float factor)
{
	return glm::mix(lastPosition, position, factor);
}

glm::quat PhysicsController::interpolate(glm::quat lastRotation, glm::quat rotation, float factor)
{
	return glm::slerp(lastRotation, rotation, factor);
}