#include "physics_bridge.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/component_wise.hpp>

#include "../src/core/utils/console.h"
#include "../src/core/physics/rigidbody/rigidbody.h"
#include "../src/core/physics/utils/px_translator.h"

using namespace physx;

PhysicsBridge::PhysicsBridge(PxPhysics*& physics, PxScene*& scene) : physics(physics), 
scene(scene), 
defaultMaterial(nullptr) 
{
};

void PhysicsBridge::setup() {
	defaultMaterial = createMaterial(physics, 0.6f, 0.6f, 0.55f);
}

void PhysicsBridge::constructBoxCollider(Registry& reg, Entity ent) {
	// Get components
	TransformComponent& transform = get<TransformComponent>(reg, ent);
	BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);

	// Create box collider
	boxCollider.size = transform.scale;
	boxCollider.material = defaultMaterial;
	boxCollider.shape = createBoxShape(physics, boxCollider.material, boxCollider.size);

	// Attach shape to rigidbody if existing
	try_rbAttachShape(reg, ent, boxCollider.shape);
}

void PhysicsBridge::destroyBoxCollider(Registry& reg, Entity ent) {
	// Get components
	BoxColliderComponent& boxCollider = get<BoxColliderComponent>(reg, ent);

	// Remove shape from rigidbody if existing
	try_rbDetachShape(reg, ent, boxCollider.shape);

	// Release shape
	boxCollider.shape->release();
}

void PhysicsBridge::constructSphereCollider(Registry& reg, Entity ent)
{
	// Get components
	SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);
	TransformComponent& transform = get<TransformComponent>(reg, ent);

	// Create sphere collider
	sphereCollider.radius = glm::compMax(transform.scale);
	sphereCollider.material = defaultMaterial;
	sphereCollider.shape = createSphereShape(physics, sphereCollider.material, sphereCollider.radius);

	// Attach shape to rigidbody if existing
	try_rbAttachShape(reg, ent, sphereCollider.shape);
}

void PhysicsBridge::destroySphereCollider(Registry& reg, Entity ent) {
	// Get components
	SphereColliderComponent& sphereCollider = get<SphereColliderComponent>(reg, ent);

	// Remove shape from rigidbody if existing
	try_rbDetachShape(reg, ent, sphereCollider.shape);

	// Release shape
	sphereCollider.shape->release();
}

void PhysicsBridge::constructRigidbody(Registry& reg, Entity ent) {
	// Get components
	RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);
	TransformComponent& transform = get<TransformComponent>(reg, ent);

	// Create rigidbody
	PxRigidDynamic* rbActor = createDynamicRigidbody(physics, scene, transform.position, transform.rotation);

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

void PhysicsBridge::destroyRigidbody(Registry& reg, Entity ent) {
	// Get components
	RigidbodyComponent& rigidbody = get<RigidbodyComponent>(reg, ent);

	// Remove rigidbody from scene
	scene->removeActor(*rigidbody.actor);

	// Release rigidbody
	rigidbody.actor->release();
}

PxMaterial* PhysicsBridge::createMaterial(PxPhysics*& physics, float staticFriction, float dynamicFriction, float restitution)
{
	PxMaterial* material = physics->createMaterial(staticFriction, dynamicFriction, restitution);

	material->setFrictionCombineMode(PxCombineMode::eAVERAGE);
	material->setRestitutionCombineMode(PxCombineMode::eAVERAGE);

	return material;
}

PxShape* PhysicsBridge::createBoxShape(PxPhysics*& physics, PxMaterial*& material, const glm::vec3& size)
{
	return physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *material);
}

PxShape* PhysicsBridge::createSphereShape(PxPhysics*& physics, PxMaterial*& material, float radius)
{
	return physics->createShape(PxSphereGeometry(radius), *material);
}

PxRigidDynamic* PhysicsBridge::createDynamicRigidbody(PxPhysics*& physics, PxScene*& scene, const glm::vec3& position, const glm::quat& rotation)
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