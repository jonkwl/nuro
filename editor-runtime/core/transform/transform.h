#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

#include "../core/ecs/components.h"

namespace Transform
{

	// Computes and updates the transformation matrices of given transform component
	void evaluate(TransformComponent& transform, const glm::mat4 viewProjection);

	// Transform related direction vectors
	glm::vec3 forward(TransformComponent& transform);
	glm::vec3 backward(TransformComponent& transform);
	glm::vec3 right(TransformComponent& transform);
	glm::vec3 left(TransformComponent& transform);
	glm::vec3 up(TransformComponent& transform);
	glm::vec3 down(TransformComponent& transform);

	// Modulate rotation
	void rotate(TransformComponent& transform, float degrees, glm::vec3 axis);
	void rotateX(TransformComponent& transform, float degrees);
	void rotateY(TransformComponent& transform, float degrees);
	void rotateZ(TransformComponent& transform, float degrees);

	// Rotation conversion
	glm::quat fromEuler(float x, float y, float z);

};