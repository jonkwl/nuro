#pragma once

#include <entt.hpp>
#include <PxPhysicsAPI.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/component_wise.hpp>

#include "../core/utils/console.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/physics/rigidbody/rigidbody.h"
#include "../core/physics/utils/px_translator.h"
#include "../core/physics/core/physics_factory.h"

class PhysicsEcsListener
{
public:
	PhysicsEcsListener(physx::PxPhysics*& physics, physx::PxScene*& scene) : 
		physics(physics), scene(scene), defaultMaterial(nullptr) {};

	void setup() {
		defaultMaterial = PhysicsFactory::createMaterial(physics, 0.6f, 0.6f, 0.55f);

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
	inline bool has(Registry& registry, Entity entity) {
		return registry.any_of<T>(entity);
	}
	
	// Returns reference to specific component from given entity in given registry
	template <typename T>
	inline T& get(Registry& registry, Entity entity) {
		return registry.get<T>(entity);
	}

public:

	//
	// HELPERS
	//

	// Attach given shape to entities rigidbody if it already has one
	void try_rbAttachShape(Registry& reg, Entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->attachShape(*shape);
		}
	}

	// Detach given shape from entities rigidbody if it already has one
	void try_rbDetachShape(Registry& reg, Entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->detachShape(*shape);
		}
	}

	// Attach any colliders already added to entity to given rigidbody actor
	void try_rbAttachExistingColliders(Registry& reg, Entity ent, physx::PxRigidDynamic* rb) {

		// Attach box collider if existing
		if (has<BoxColliderComponent>(reg, ent)) {
			rb->attachShape(*get<BoxColliderComponent>(reg, ent).shape);
		}

		// Attach sphere collider if existing
		if (has<SphereColliderComponent>(reg, ent)) {
			rb->attachShape(*get<SphereColliderComponent>(reg, ent).shape);
		}

	}

public:

	//
	// EVENTS
	//

	void constructBoxCollider(Registry& reg, Entity ent)
	{
		// Get components
		BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);
		TransformComponent& transform = get<TransformComponent>(reg, ent);

		// Create box collider
		boxCollider.size = transform.scale;
		boxCollider.material = defaultMaterial;
		boxCollider.shape = PhysicsFactory::createBoxShape(physics, boxCollider.material, boxCollider.size);

		// Attach shape to rigidbody if existing
		try_rbAttachShape(reg, ent, boxCollider.shape);
	}

	void destroyBoxCollider(Registry& reg, Entity ent)
	{
		// Get components
		BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);

		// Remove shape from rigidbody if existing
		try_rbDetachShape(reg, ent, boxCollider.shape);

		// Release shape
		boxCollider.shape->release();
	}

	void constructSphereCollider(Registry& reg, Entity ent)
	{
		// Get components
		SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);
		TransformComponent& transform = get<TransformComponent>(reg, ent);

		// Create sphere collider
		sphereCollider.radius = glm::compMax(transform.scale);
		sphereCollider.material = defaultMaterial;
		sphereCollider.shape = PhysicsFactory::createSphereShape(physics, sphereCollider.material, sphereCollider.radius);

		// Attach shape to rigidbody if existing
		try_rbAttachShape(reg, ent, sphereCollider.shape);
	}

	void destroySphereCollider(Registry& reg, Entity ent)
	{
		// Get components
		SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);

		// Remove shape from rigidbody if existing
		try_rbDetachShape(reg, ent, sphereCollider.shape);

		// Release shape
		sphereCollider.shape->release();
	}

	void constructRigidbody(Registry& reg, Entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);
		TransformComponent& transform = get<TransformComponent>(reg, ent);

		// Create rigidbody
		physx::PxRigidDynamic* rbActor = PhysicsFactory::createDynamicRigidbody(physics, scene, transform.position, transform.rotation);
		
		// Set rigidbody actor
		rigidbody.actor = rbActor;

		// Attach all existing colliders to rigidbody actor
		try_rbAttachExistingColliders(reg, ent, rbActor);

		// Set rigidbody actor specific default values
		Rigidbody::setCollisionDetection(rigidbody, rigidbody.collisionDetection);
		Rigidbody::setResistance(rigidbody, rigidbody.resistance);
		Rigidbody::setAngularResistance(rigidbody, rigidbody.angularResistance);
		Rigidbody::setGravity(rigidbody, rigidbody.gravity);
		Rigidbody::setKinematic(rigidbody, rigidbody.kinematic);

		// Set rigidbodies initial transform
		rigidbody.position = transform.position;
		rigidbody.rotation = transform.rotation;
	}

	void destroyRigidbody(Registry& reg, Entity ent)
	{
		// Get components
		RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);

		// Remove rigidbody from scene
		scene->removeActor(*rigidbody.actor);

		// Release rigidbody
		rigidbody.actor->release();
	}

private:
	physx::PxPhysics*& physics;
	physx::PxScene*& scene;

	physx::PxMaterial* defaultMaterial; // tmp

};