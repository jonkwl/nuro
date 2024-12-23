#pragma once

#include <PxPhysicsAPI.h>
#include <glm.hpp>

#include "../core/physics/events/physics_observer.hpp"

class PhysicsController
{
public:
	PhysicsController();

	void create(); // Create physics
	void destroy(); // Destroy physics

	void step(float delta); // Steps physics and performs simulation iterations

private:
	void simulate(float delta); // Simulates physics iteration

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

	PhysicsObserver observer;

private:
	const physx::PxReal timeStep;
	const physx::PxVec3 gravity;

private:
	float accumulatedTime;

};