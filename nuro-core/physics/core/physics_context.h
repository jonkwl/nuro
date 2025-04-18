#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <ecs/ecs_collection.h>

#include <physics/core/physics_bridge.h>

class PhysicsContext
{
public:
	PhysicsContext();

	void create(); // Create physics
	void destroy(); // Destroy physics

	// Steps physics and performs simulation iterations
	void step();

private:
	// Simulates physics iteration
	void simulate();

	// Apply transform of physics rigidbody on given rigidbody component
	void syncRigidbodyComponent(RigidbodyComponent& rigidbody);

	// Apply position and rotation of given rigidbody component to given transform component
	void syncTransformComponent(float delta, TransformComponent& transform, RigidbodyComponent& rigidbody);

	glm::vec3 interpolate(glm::vec3 lastPosition, glm::vec3 position, float factor);
	glm::quat interpolate(glm::quat lastRotation, glm::quat rotation, float factor);

private:
	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;
	physx::PxFoundation* foundation;
	physx::PxPhysics* physics;
	physx::PxDefaultCpuDispatcher* dispatcher;
	physx::PxScene* scene;
	physx::PxPvd* pvd;

	PhysicsBridge bridge;

	const physx::PxReal timeStep;
	const physx::PxVec3 gravity;
	double accumulatedTime;

};