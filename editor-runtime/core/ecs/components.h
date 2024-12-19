#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class Mesh;

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

	MeshRendererComponent(Mesh& mesh);

	Mesh& mesh; // Mesh render target

};

struct CameraComponent {

	CameraComponent();

	float fov; // Cameras y fov in degrees
	float near; // Near clipping
	float far; // Far clipping

};