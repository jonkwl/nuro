#include "physics_factory.h"

#include "../core/physics/utils/px_translator.h"

using namespace physx;

namespace PhysicsFactory
{

	//
	// MATERIALS
	//

	PxMaterial* createMaterial(PxPhysics*& physics, float staticFriction, float dynamicFriction, float restitution)
	{
		PxMaterial* material = physics->createMaterial(staticFriction, dynamicFriction, restitution);
		
		material->setFrictionCombineMode(PxCombineMode::eAVERAGE);
		material->setRestitutionCombineMode(PxCombineMode::eAVERAGE);

		return material;
	}

	//
	// SHAPES
	//

	PxShape* createBoxShape(PxPhysics*& physics, physx::PxMaterial*& material, const glm::vec3& size)
	{
		return physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *material);
	}

	PxShape* createSphereShape(PxPhysics*& physics, physx::PxMaterial*& material, float radius)
	{
		return physics->createShape(PxSphereGeometry(radius), *material);
	}

	//
	// RIGIDBODIES
	//

	PxRigidDynamic* createDynamicRigidbody(PxPhysics*& physics, PxScene*& scene, const glm::vec3& position, const glm::quat& rotation)
	{
		// Create transform
		PxVec3 _position = PxTranslator::convert(position);
		PxQuat _rotation = PxTranslator::convert(rotation);

		// Create rigidbody actor
		PxRigidDynamic* rbActor = physics->createRigidDynamic(PxTransform(_position, _rotation));

		// Setup rigidbody
		float density = 1.0f;
		PxRigidBodyExt::updateMassAndInertia(*rbActor, density);

		// Add rigidbody actor to scene
		scene->addActor(*rbActor);

		return rbActor;
	}
};