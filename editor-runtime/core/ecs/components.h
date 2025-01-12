#pragma once

#include <cstdint>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <PxPhysicsAPI.h>

#include "../core/rendering/model/mesh.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/physics/rigidbody/rigidbody_enums.h"

struct TransformComponent {

	// Current position of transform
	glm::vec3 position = glm::vec3(0.0f);

	// Current rotation of transform
	glm::quat rotation = glm::identity<glm::quat>();

	// Current scale of transform
	glm::vec3 scale = glm::vec3(1.0f);

	// Optional parent
	TransformComponent* parent;

	// Latest computed model-view-projection matrix
	glm::mat4 mvp = glm::mat4(1.0f);

	// Latest computed model matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Latest computed normal matrix
	glm::mat4 normal = glm::mat4(1.0f);

};

static inline Mesh emptyMesh;
struct MeshRendererComponent {

	MeshRendererComponent() : mesh(emptyMesh), material(nullptr) {};
	MeshRendererComponent(Mesh& mesh, IMaterial* material) : mesh(mesh), material(material) {};

	// Mesh render target
	Mesh& mesh;

	// Mesh material | Unsafe!
	IMaterial* material;
};

struct CameraComponent {

	// Cameras y fov in degrees
	float fov = 70.0f;

	// Near clipping
	float near = 0.3f;

	// Far clipping
	float far = 1000.0f;

};

struct VelocityComponent {

	// Intensity of the velocity impact
	float intensity = 1.0f;

	// Cached model matrix from last frame
	glm::mat4 lastModel = glm::mat4(1.0f);

};

struct BoxColliderComponent {

	// Offset relative to transform origin
	glm::vec3 center = glm::vec3(0.0f);

	// Scale relative to transforms scale
	glm::vec3 size = glm::vec3(1.0f);

	// Physics material
	physx::PxMaterial* material = nullptr;

	// Shape handle
	physx::PxShape* shape = nullptr;

};

struct SphereColliderComponent {

	// Offset relative to transform origin
	glm::vec3 center = glm::vec3(0.0f);

	// Radius relative to transforms scale
	float radius = 1.0f;

	// Physics material
	physx::PxMaterial* material = nullptr;

	// Physics backend shape handle
	physx::PxShape* shape = nullptr;

};

struct RigidbodyComponent {

	// Interpolation mode
	RB_Interpolation interpolation = RB_Interpolation::INTERPOLATE;

	// Collision detection mode
	RB_CollisionDetection collisionDetection = RB_CollisionDetection::DISCRETE;
	
	// Mass (Determines the rigidbody density relative to the volume of its colliders)
	float mass = 1.0f;

	// Linear resistance coefficient
	float resistance = 0.0f;

	// Angular resistance coefficient
	float angularResistance = 0.05f;

	// Sets if the rigidobody is being affected by gravity
	bool gravity = true;

	// Sets rigidbody to kinematic, not being affected by physics if set
	bool kinematic = false;

	// Current linear velocity
	glm::vec3 velocity = glm::vec3(0.0f);

	// Current angular velocity 
	glm::vec3 angularVelocity = glm::vec3(0.0f);

	// Current rigidbody physics-internal position
	glm::vec3 position = glm::vec3(0.0f);

	// Current rigidbody physics-internal rotation
	glm::quat rotation = glm::identity<glm::quat>();

	// Physics backend actor handle
	physx::PxRigidDynamic* actor = nullptr;

};