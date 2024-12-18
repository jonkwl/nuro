#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class Mesh;

struct TransformComponent {

	TransformComponent();

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::mat4 modelMatrix; // Model matrix cache for current frame
	glm::mat4 mvpMatrix; // MVP matrix cache for current frame
	glm::mat4 normalMatrix; // Normal matrix cache for current frame

};

struct MeshRendererComponent {

	MeshRendererComponent();

	Mesh* mesh;

};

struct CameraComponent {

	CameraComponent();

	float fov;
	float near;
	float far;

};