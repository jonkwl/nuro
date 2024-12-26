#pragma once

#include <cstdint>
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <PxPhysicsAPI.h>

#include "../core/rendering/model/mesh.h"
#include "../core/rendering/material/imaterial.h"

struct TransformComponent {

	// Transform data
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::identity<glm::quat>();
	glm::vec3 scale = glm::vec3(1.0f);

	// Matrix cache
	glm::mat4 mvp = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 normal = glm::mat4(1.0f);

};

static inline Mesh emptyMesh;
struct MeshRendererComponent {

	MeshRendererComponent() : mesh(emptyMesh), material(nullptr) {};
	MeshRendererComponent(Mesh& mesh, IMaterial* material) : mesh(mesh), material(material) {};

	Mesh& mesh; // Mesh render target
	IMaterial* material; // Mesh material | Unsafe!
};

struct CameraComponent {

	float fov = 70.0f; // Cameras y fov in degrees
	float near = 0.3f; // Near clipping
	float far = 1000.0f; // Far clipping

};

struct VelocityComponent {

	float intensity = 1.0f; // Velocity impact
	glm::mat4 lastModel = glm::mat4(1.0f); // Model matrix cache

};

struct BoxColliderComponent {

	glm::vec3 center = glm::vec3(0.0f); // Offset relative to transform origin
	glm::vec3 size = glm::vec3(1.0f); // Scale relative to transforms scale

	physx::PxMaterial* material = nullptr; // Physics material
	physx::PxShape* shape = nullptr; // Shape handle

};

struct SphereColliderComponent {

	glm::vec3 center = glm::vec3(0.0f); // Colliders offset relative to transform origin
	float radius = 1.0f; // Radius relative to transforms scale

	physx::PxMaterial* material = nullptr; // Physics material
	physx::PxShape* shape = nullptr; // Shape handle

};

enum class RB_Interpolation {
	NONE,
	INTERPOLATE,
	EXTRAPOLATE
};

enum class RB_CollisionDetection {
	DISCRETE,
	CONTINUOUS,
	CONTINUOUS_SPECULATIVE
};

struct RigidbodyComponent {

	RB_Interpolation interpolation = RB_Interpolation::INTERPOLATE; // Rigidbody interpolation mode
	RB_CollisionDetection collisionDetection = RB_CollisionDetection::DISCRETE; // Rigidbody collision detection mode
	
	float mass = 1.0f; // Rigidbody mass
	float resistance = 0.0f; // Resistance coefficient
	float angularResistance = 0.05f; // Angular resistance coefficient

	bool gravity = true; // Gets affected by gravity if set
	bool kinematic = false; // Rigidbody not being affected by physics if set

	glm::vec3 velocity = glm::vec3(0.0f); // Current linear velocity
	glm::vec3 angularVelocity = glm::vec3(0.0f); // Current angular velocity 

	glm::vec3 position = glm::vec3(0.0f); // Current position
	glm::quat rotation = glm::identity<glm::quat>(); // Current rotation

	physx::PxRigidDynamic* actor = nullptr; // Actor handle

};