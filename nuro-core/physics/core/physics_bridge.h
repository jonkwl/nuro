#pragma once

#include <entt/entt.hpp>
#include <PxPhysicsAPI.h>

#include <ecs/ecs_collection.h>

class PhysicsBridge
{
public:
	PhysicsBridge(physx::PxPhysics*& physics, physx::PxScene*& scene);
	void setup();

	//
	// CREATE / DESTROY PHYSICS RELATED COMPONENTS
	//

	void constructBoxCollider(Registry& reg, Entity ent);
	void destroyBoxCollider(Registry& reg, Entity ent);

	void constructSphereCollider(Registry& reg, Entity ent);
	void destroySphereCollider(Registry& reg, Entity ent);

	void constructRigidbody(Registry& reg, Entity ent);
	void destroyRigidbody(Registry& reg, Entity ent);
	 
private:

	//
	// PHYSICS BACKEND WRAPPERS
	//

	physx::PxMaterial* createMaterial(physx::PxPhysics*& physics, float staticFriction = 0.6f, float dynamicFriction = 0.6, float restitution = 0.0f);
	physx::PxShape* createBoxShape(physx::PxPhysics*& physics, physx::PxMaterial*& material, const glm::vec3& size);
	physx::PxShape* createSphereShape(physx::PxPhysics*& physics, physx::PxMaterial*& material, float radius);
	physx::PxRigidDynamic* createDynamicRigidbody(physx::PxPhysics*& physics, physx::PxScene*& scene, const glm::vec3& position, const glm::quat& rotation);

private:

	//
	// ECS HELPERS
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

	// Attach given shape to entities rigidbody if it already has one
	inline void try_rbAttachShape(Registry& reg, Entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->attachShape(*shape);
		}
	}

	// Detach given shape from entities rigidbody if it already has one
	inline void try_rbDetachShape(Registry& reg, Entity ent, physx::PxShape* shape) {
		if (has<RigidbodyComponent>(reg, ent)) {
			get<RigidbodyComponent>(reg, ent).actor->detachShape(*shape);
		}
	}

	// Attach any colliders already added to entity to given rigidbody actor
	inline void try_rbAttachExistingColliders(Registry& reg, Entity ent, physx::PxRigidDynamic* rb) {

		// Attach box collider if existing
		if (has<BoxColliderComponent>(reg, ent)) {
			rb->attachShape(*get<BoxColliderComponent>(reg, ent).shape);
		}

		// Attach sphere collider if existing
		if (has<SphereColliderComponent>(reg, ent)) {
			rb->attachShape(*get<SphereColliderComponent>(reg, ent).shape);
		}

	}

private:
	physx::PxPhysics*& physics;
	physx::PxScene*& scene;

	physx::PxMaterial* defaultMaterial; // tmp

};