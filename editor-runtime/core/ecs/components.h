#pragma once

#include <cstdint>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

#include "../core/rendering/model/mesh.h"

struct TransformComponent {

	// Transform data
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat();
	glm::vec3 scale = glm::vec3(1.0f);

	// Matrix cache
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = glm::mat4(1.0f);


};

inline Mesh nullMesh;
struct MeshRendererComponent {

	MeshRendererComponent() : mesh(nullMesh) {};
	MeshRendererComponent(Mesh& mesh) : mesh(mesh) {};

	Mesh& mesh; // Mesh render target

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