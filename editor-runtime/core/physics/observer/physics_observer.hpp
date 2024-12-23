#pragma once

#include <entt.hpp>
#include <PxPhysicsAPI.h>

#include "../core/utils/log.h"
#include "../core/ecs/components.hpp"
#include "../core/physics/utils/px_translator.h"

class PhysicsObserver
{
public:
	PhysicsObserver(physx::PxPhysics*& physics, physx::PxScene*& scene, physx::PxMaterial*& defaultMaterial) : 
		physics(physics), scene(scene), defaultMaterial(defaultMaterial) {};

public:

	//
	// ECS SHORTHANDS
	//

	// Returns if given entity in given registry has specific component
	template <typename T>
	inline bool has(entt::registry& registry, entt::entity entity) {
		return registry.any_of<T>(entity);
	}
	
	// Returns reference to specific component from given entity in given registry
	template <typename T>
	inline T& get(entt::registry& registry, entt::entity entity) {
		return registry.get<T>(entity);
	}

public:

	//
	// HELPERS
	//

	// Attach given shape to entities rigidbody if it already has one
	void try_rbAttachShape(entt::registry& reg, entt::entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->attachShape(*shape);
		}
	}

	// Detach given shape from entities rigidbody if it already has one
	void try_rbDetachShape(entt::registry& reg, entt::entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->detachShape(*shape);
		}
	}

	// Attach any colliders already added to entity to given rigidbody actor
	void try_rbAttachExistingColliders(entt::registry& reg, entt::entity ent, physx::PxRigidActor* rb) {

		// Attach box collider if existing
		if (has<BoxColliderComponent>(reg, ent)) {
			rb->attachShape(*get<BoxColliderComponent>(reg, ent).shape);
		}

	}

public:

	//
	// EVENTS
	//

	void constructBoxCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		BoxColliderComponent& collider = get<BoxColliderComponent>(reg, ent);

		// Create shape
		physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(collider.size.x / 2, collider.size.y / 2, collider.size.z / 2), *defaultMaterial);
		collider.shape = shape;

		// Attach shape to rigidbody if existing
		try_rbAttachShape(reg, ent, shape);

		Log::printWarning("Physics Observer", "CONSTRUCTED BOX COLLIDER");
	}

	void destroyBoxCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		BoxColliderComponent& collider = get<BoxColliderComponent>(reg, ent);

		// Remove shape from rigidbody if existing
		try_rbDetachShape(reg, ent, collider.shape);

		// Release shape
		collider.shape->release();

		Log::printWarning("Physics Observer", "DESTROYED BOX COLLIDER");
	}

	void constructRigidbody(entt::registry& reg, entt::entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);
		TransformComponent& transform = get<TransformComponent>(reg, ent);

		// Create transform
		physx::PxVec3 position = PxTranslator::convert(transform.position);
		physx::PxQuat rotation = PxTranslator::convert(transform.rotation);

		// Create rigidbody actor
		physx::PxRigidDynamic* actor = physics->createRigidDynamic(physx::PxTransform(position, rotation));

		// Setup rigidbody
		float density = 1.0f;
		physx::PxRigidBodyExt::updateMassAndInertia(*actor, density);
		
		// Attach all existing colliders to rigidbody
		try_rbAttachExistingColliders(reg, ent, actor);
		
		// Add rigidbody actor to scene
		scene->addActor(*actor);

		// Set rigidbody component data
		rigidbody.position = transform.position;
		rigidbody.rotation = transform.rotation;
		rigidbody.actor = actor;

		Log::printWarning("Physics Observer", "CONSTRUCTED RIGIDBODY");
	}

	void destroyRigidbody(entt::registry& reg, entt::entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);

		// Destroy actor
		physx::PxRigidActor* actor = rigidbody.actor;
		scene->removeActor(*actor);
		actor->release();
	}

private:
	physx::PxPhysics*& physics;
	physx::PxScene*& scene;
	physx::PxMaterial*& defaultMaterial;

};