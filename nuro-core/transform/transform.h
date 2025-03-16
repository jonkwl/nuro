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

	// Returns if a transform is a root transform
	bool isRoot(TransformComponent& transform);

	// Returns if a transform has a parent
	bool hasParent(TransformComponent& transform);

	// Returns a transforms parent; make sure transform has parent before using!
	TransformComponent& fetchParent(TransformComponent& transform);

	// Updates a transforms model matrix
	void evaluate(TransformComponent& transform);

	// Updates a transforms model matrix relative to the given parent
	void evaluate(TransformComponent& transform, TransformComponent& parent);

	// Updates a transforms model-view-projection matrix
	void updateMvp(TransformComponent& transform, const glm::mat4& viewProjection);

	//
	// TRANSFORMATION
	//

	void setPosition(TransformComponent& transform, const glm::vec3& position, Space space = Space::LOCAL);
	void setRotation(TransformComponent& transform, const glm::quat& rotation, Space space = Space::LOCAL);
	void setEulerAngles(TransformComponent& transform, const glm::vec3& eulerAngles, Space space = Space::LOCAL);
	void setScale(TransformComponent& transform, const glm::vec3& scale, Space space = Space::LOCAL);

	glm::vec3 getPosition(TransformComponent& transform, Space space = Space::LOCAL);
	glm::quat getRotation(TransformComponent& transform, Space space = Space::LOCAL);
	glm::vec3 getEulerAngles(TransformComponent& transform, Space space = Space::LOCAL);
	glm::vec3 getScale(TransformComponent& transform, Space space = Space::LOCAL);

	void translate(TransformComponent& transform, const glm::vec3& position, Space space = Space::LOCAL);
	void rotate(TransformComponent& transform, const glm::quat& rotation, Space space = Space::LOCAL);
	void scale(TransformComponent& transform, const glm::vec3& scale, Space space = Space::LOCAL);

	//
	// HELPERS
	//

	// Returns the forward direction vector of the transform
	glm::vec3 forward(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns the backward direction vector of the transform
	glm::vec3 backward(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns the right direction vector of the transform
	glm::vec3 right(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns the left direction vector of the transform
	glm::vec3 left(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns the up direction vector of the transform
	glm::vec3 up(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns the down direction vector of the transform
	glm::vec3 down(TransformComponent& transform, Space space = Space::LOCAL);

	// Returns a quaternion that will rotate an object at given position to face the target position
	glm::quat lookAt(const glm::vec3& position, const glm::vec3& target);

	// Converts euler angles in degrees to a quaternion
	glm::quat toQuat(glm::vec3 eulerAngles);

	// Convers a quaternion to euler angles in degrees
	glm::vec3 toEuler(const glm::quat& quaternion);

};