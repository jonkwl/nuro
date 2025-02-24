#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ecs/components.h>

namespace Transform
{

	// Computes and updates the transformation matrices of given transform component
	void evaluate(TransformComponent& transform, const glm::mat4 viewProjection);

	// Returns the forward direction vector of the transform in world space
	glm::vec3 forward(const TransformComponent& transform);

	// Returns the backward direction vector of the transform in world space
	glm::vec3 backward(const TransformComponent& transform);

	// Returns the right direction vector of the transform in world space
	glm::vec3 right(const TransformComponent& transform);

	// Returns the left direction vector of the transform in world space
	glm::vec3 left(const TransformComponent& transform);

	// Returns the up direction vector of the transform in world space
	glm::vec3 up(const TransformComponent& transform);

	// Returns the down direction vector of the transform in world space
	glm::vec3 down(const TransformComponent& transform);

	// Returns new rotation of transform rotated by the specified degrees around the given axis in world space
	glm::quat rotate(const TransformComponent& transform, float degrees, glm::vec3 axis);

	// Returns new rotation of transform rotated by the specified degrees around its local x-axis
	glm::quat rotateX(const TransformComponent& transform, float degrees);

	// Returns new rotation of transform rotated by the specified degrees around its local y-axis
	glm::quat rotateY(const TransformComponent& transform, float degrees);

	// Returns new rotation of transform rotated by the specified degrees around its local z-axis
	glm::quat rotateZ(const TransformComponent& transform, float degrees);

	// Returns a quaternion that will rotate the transform to face the target transform
	glm::quat lookAt(const TransformComponent& transform, const TransformComponent& target);

	// Returns a quaternion that will rotate some transform at given positionto face the target transform
	glm::quat lookFromAt(const glm::vec3 from, const TransformComponent& target);

	// Converts euler angles in degrees to a quaternion
	glm::quat fromEuler(float x, float y, float z);

};