#pragma once

#include <entt.hpp>
#include <PxPhysicsAPI.h>

#include "../core/utils/log.h"
#include "../core/ecs/components.h"
#include "../core/physics/utils/px_translator.h"
#include "../core/physics/core/physics_collection.h"

class PhysicsObserver
{
public:
	PhysicsObserver(physx::PxPhysics*& physics, physx::PxScene*& scene) : 
		physics(physics), scene(scene), defaultMaterial(nullptr) {};

	void setup() {
		defaultMaterial = PhysicsCollection::createMaterial(physics, 0.6f, 0.6f, 0.55f);

		// tmp
		physx::PxRigidStatic* plane = PxCreatePlane(*physics, physx::PxPlane(0.0f, 1.0f, 0.0f, 10.0f), *defaultMaterial);
		scene->addActor(*plane);
	}

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
			PhysicsCollection::attachCollider(rb, get<BoxColliderComponent>(reg, ent).shape);
		}

		// Attach sphere collider if existing
		if (has<SphereColliderComponent>(reg, ent)) {
			PhysicsCollection::attachCollider(rb, get<SphereColliderComponent>(reg, ent).shape);
		}

	}

public:

	//
	// EVENTS
	//

	void constructBoxCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);

		// Create shape
		boxCollider.material = defaultMaterial;
		boxCollider.shape = PhysicsCollection::createBoxCollider(physics, boxCollider);

		// Attach shape to rigidbody if existing
		try_rbAttachShape(reg, ent, boxCollider.shape);
	}

	void destroyBoxCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);

		// Remove shape from rigidbody if existing
		try_rbDetachShape(reg, ent, boxCollider.shape);

		// Destroy collider
		PhysicsCollection::destroyCollider(boxCollider.shape);
	}

	void constructSphereCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);

		// Create shape
		sphereCollider.material = defaultMaterial;
		sphereCollider.shape = PhysicsCollection::createSphereCollider(physics, sphereCollider);

		// Attach shape to rigidbody if existing
		try_rbAttachShape(reg, ent, sphereCollider.shape);
	}

	void destroySphereCollider(entt::registry& reg, entt::entity ent)
	{
		// Get components
		SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);

		// Remove shape from rigidbody if existing
		try_rbDetachShape(reg, ent, sphereCollider.shape);

		// Destroy collider
		PhysicsCollection::destroyCollider(sphereCollider.shape);
	}

	void constructRigidbody(entt::registry& reg, entt::entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);
		TransformComponent& transform = get<TransformComponent>(reg, ent);

		// Create rigidbody
		physx::PxRigidActor* rbActor = PhysicsCollection::createRigidbody(physics, scene, transform, rigidbody);

		// Attach all existing colliders to rigidbody actor
		try_rbAttachExistingColliders(reg, ent, rbActor);

		// Set rigidbody component data
		rigidbody.position = transform.position;
		rigidbody.rotation = transform.rotation;
		rigidbody.actor = rbActor;
	}

	void destroyRigidbody(entt::registry& reg, entt::entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);

		// Destroy rigidbody
		PhysicsCollection::destroyRigidbody(scene, rigidbody.actor);
	}

private:
	physx::PxPhysics*& physics;
	physx::PxScene*& scene;

	physx::PxMaterial* defaultMaterial;

};