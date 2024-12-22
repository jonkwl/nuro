#pragma once

#include <cstdint>
#include <glm.hpp>
#include <gtc/quaternion.hpp>

#include "../core/rendering/model/mesh.h"

struct TransformComponent {

	TransformComponent();

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	// Matrix cache
	glm::mat4 model;
	glm::mat4 mvp;

};

struct MeshRendererComponent {

	MeshRendererComponent();
	MeshRendererComponent(Mesh& mesh);

	Mesh& mesh; // Mesh render target

};

struct CameraComponent {

	CameraComponent();

	float fov; // Cameras y fov in degrees
	float near; // Near clipping
	float far; // Far clipping

};

struct VelocityComponent {

	VelocityComponent();

	float intensity; // Velocity impact

	// Matrix cache
	glm::mat4 lastModel;

};