#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <PxPhysicsAPI.h>
#include <glm/gtc/quaternion.hpp>

#include <rendering/model/mesh.h>
#include <rendering/material/imaterial.h>
#include <physics/rigidbody/rigidbody_enums.h>

using Entity = entt::entity;

struct TransformComponent {

	//
	// GENERAL
	//

	// Unique id of entity
	uint32_t id = 0;

	// Name of entity
	std::string name;

	// Set if transform was modified and needs to be evaluated (initially set)
	bool modified = true;

	//
	// TRANSFORMATION
	//

	// Current position of transform in local space
	glm::vec3 position = glm::vec3(0.0f);

	// Current rotation of transform in local space
	glm::quat rotation = glm::identity<glm::quat>();

	// Current reflected euler angles of transform in local space
	glm::vec3 eulerAngles = glm::vec3(0.0f);

	// Current scale of transform in local space
	glm::vec3 scale = glm::vec3(1.0f);

	//
	// HIERARCHY
	//

	// Optional parent
	Entity parent = entt::null;

	// List of children
	std::vector<Entity> children;

	//
	// MATRIX CACHE
	//

	// Latest evaluated model matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Latest evaluated normal matrix
	glm::mat4 normal = glm::mat4(1.0f);

	// Latest evaluated model-view-projection matrix
	glm::mat4 mvp = glm::mat4(1.0f);

};

struct MeshRendererComponent {

	MeshRendererComponent() : enabled(true), mesh(nullptr), material(nullptr) {};
	MeshRendererComponent(const Mesh* mesh, IMaterial* material) : enabled(true), mesh(mesh), material(material) {};
	
	// Set if component is enabled
	bool enabled = true;

	// Mesh render target
	const Mesh* mesh = nullptr;

	// Mesh material - TMP - UNSAFE!
	const IMaterial* material = nullptr;
};

struct CameraComponent {

	// Set if component is enabled
	bool enabled = true;

	// Cameras y fov in degrees
	float fov = 70.0f;

	// Near clipping
	float near = 0.3f;

	// Far clipping
	float far = 1000.0f;

};

struct DirectionalLightComponent {

	// Set if component is enabled
	bool enabled = true;

	// Intensity of the directional light
	float intensity = 1.0f;

	// Color directional light is emitting
	glm::vec3 color = glm::vec3(1.0f);
};

struct PointLightComponent {

	// Set if component is enabled
	bool enabled = true;

	// Intensity of the point light
	float intensity = 1.0f;

	// Color point light is emitting
	glm::vec3 color = glm::vec3(1.0f);

	// Range of the point light
	float range = 10.0f;

	// Falloff of the point light
	float falloff = 5.0f;
};

struct SpotlightComponent {

	// Set if component is enabled
	bool enabled = true;

	// Intensity of the spotlight
	float intensity = 1.0f;

	// Color spotlight is emitting
	glm::vec3 color = glm::vec3(1.0f);

	// Range of the spotlight
	float range = 10.0f;

	// Falloff of the spotlight
	float falloff = 5.0f;

	// Inner cutoff angle in degrees of the spotlight
	float innerAngle = 45.0f;

	// Outer cutoff angle in degrees of the spotlight
	float outerAngle = 60.0f;
};

struct VelocityComponent {

	// Set if component is enabled
	bool enabled = true;

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