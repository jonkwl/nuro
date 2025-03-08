#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <ecs/components.h>

enum class Space {
	LOCAL,
	WORLD
};

namespace Transform
{

	//
	// CORE FUNCTIONALITY
	//

	// Returns if a transform has a paren
	bool hasParent(TransformComponent& transform);

	// Returns a transforms parent; make sure transform has parent before using!
	TransformComponent& fetchParent(TransformComponent& transform);

	// Evaluates the given transforms model and normal matrix if it has been modified; Note: its parent must have been evaluated before
	void evaluate(TransformComponent& transform);

	// Evaluates the transforms matrix-view-projection matrix using its current model matrix
	void createMvp(TransformComponent& transform, const glm::mat4& viewProjection);

	//
	// TRANSFORM GETTERS & SETTERS
	//

	void setPosition(TransformComponent& transform, const glm::vec3& position, Space space = Space::LOCAL);
	void setRotation(TransformComponent& transform, const glm::quat& rotation, Space space = Space::LOCAL);
	void setScale(TransformComponent& transform, const glm::vec3& scale, Space space = Space::LOCAL);

	void translate(TransformComponent& transform, const glm::vec3& position, Space space = Space::LOCAL);
	void rotate(TransformComponent& transform, const glm::quat& rotation, Space space = Space::LOCAL);
	void scale(TransformComponent& transform, const glm::vec3& scale, Space space = Space::LOCAL);

	//
	// HELPERS
	//

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

	// Returns a quaternion that will rotate the transform to face the target transform
	glm::quat lookAt(const TransformComponent& transform, const TransformComponent& target);

	// Returns a quaternion that will rotate some transform at given positionto face the target transform
	glm::quat lookFromAt(const glm::vec3 from, const TransformComponent& target);

	// Converts euler angles in degrees to a quaternion
	glm::quat fromEuler(float x, float y, float z);

};