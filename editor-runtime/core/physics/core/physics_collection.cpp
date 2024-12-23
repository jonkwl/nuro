#include "physics_collection.h"

#include "../core/physics/utils/px_translator.h"

using namespace physx;

namespace PhysicsCollection
{
	PxMaterial* createMaterial(PxPhysics*& physics, float staticFriction, float dynamicFriction, float restitution)
	{
		PxMaterial* material = physics->createMaterial(staticFriction, dynamicFriction, restitution);
		
		material->setFrictionCombineMode(PxCombineMode::eAVERAGE);
		material->setRestitutionCombineMode(PxCombineMode::eAVERAGE);

		return material;
	}

	PxShape* createBoxCollider(PxPhysics*& physics, const BoxColliderComponent& boxCollider)
	{
		return physics->createShape(PxBoxGeometry(boxCollider.size.x, boxCollider.size.y, boxCollider.size.z), *boxCollider.material);
	}

	PxShape* createSphereCollider(PxPhysics*& physics, const SphereColliderComponent& sphereCollider)
	{
		return physics->createShape(PxSphereGeometry(sphereCollider.radius), *sphereCollider.material);
	}

	void attachCollider(PxRigidActor*& actor, PxShape*& shape)
	{
		actor->attachShape(*shape);
	}

	void destroyCollider(PxShape*& shape)
	{
		shape->release();
	}

	PxRigidActor* createRigidbody(PxPhysics*& physics, PxScene*& scene, const TransformComponent& transform, const RigidbodyComponent& rigidbody)
	{
		// Create transform
		PxVec3 position = PxTranslator::convert(transform.position);
		PxQuat rotation = PxTranslator::convert(transform.rotation);

		// Create rigidbody actor
		PxRigidDynamic* rbActor = physics->createRigidDynamic(PxTransform(position, rotation));

		// Setup rigidbody
		float density = 1.0f;
		PxRigidBodyExt::updateMassAndInertia(*rbActor, density);

		// Add rigidbody actor to scene
		scene->addActor(*rbActor);

		return rbActor;
	}

	void destroyRigidbody(PxScene*& scene, PxRigidActor*& actor)
	{
		scene->removeActor(*actor);
		actor->release();
	}
};