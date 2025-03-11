#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <ecs/ecs.h>
#include <rendering/transformation/transformation.h>

namespace Transform {

	bool hasParent(TransformComponent& transform)
	{
		return transform.parent != entt::null;
	}

	TransformComponent& fetchParent(TransformComponent& transform)
	{
		return ECS::gRegistry.get<TransformComponent>(transform.parent);
	}

	void evaluate(TransformComponent& transform)
	{
		// Transform has parent
		if (hasParent(transform)) {

			TransformComponent& parent = fetchParent(transform);

			// Set transform to be modified if parent is modified
			if (parent.modified) transform.modified = true;

			// Only evaluate transform if it has been modified
			if (!transform.modified) return;

			// Evaluate transforms model matrix relative to parent
			transform.model = parent.model * Transformation::model(transform.position, transform.rotation, transform.scale);

		}
		// Transform doesn't have parent
		else {

			// Only evaluate transform if it has been modified
			if (!transform.modified) return;

			// Evaluate transforms model matrix
			transform.model = Transformation::model(transform.position, transform.rotation, transform.scale);

		}

		// Compute transforms normal matrix
		transform.normal = Transformation::normal(transform.model);
	}

	void evaluateRecursive(TransformComponent& transform)
	{
		// Evaluate transform
		evaluate(transform);

		// Evaluate children recursively
		for (Entity child : transform.children) {
			evaluateRecursive(ECS::gRegistry.get<TransformComponent>(child));
		}
	}

	void evaluateMvp(TransformComponent& transform, const glm::mat4& viewProjection)
	{
		transform.mvp = viewProjection * transform.model;
	}

	void setPosition(TransformComponent& transform, const glm::vec3& position, Space space)
	{
		// Set position in local space
		if (space == Space::LOCAL) {
			transform.position = position;
		}
		// Set position in world space
		else {

		}
		transform.modified = true;
	}

	void setRotation(TransformComponent& transform, const glm::quat& rotation, Space space)
	{
		// Set rotation in local space
		if (space == Space::LOCAL) {
			transform.rotation = rotation;
		}
		// Set rotation in world space
		else {

		}
		transform.modified = true;
	}

	void setScale(TransformComponent& transform, const glm::vec3& scale, Space space)
	{
		// Set scale in local space
		if (space == Space::LOCAL) {
			transform.scale = scale;
		}
		// Set scale in world space
		else {

		}
		transform.modified = true;
	}

	// Get position in specified space
	glm::vec3 getPosition(TransformComponent& transform, Space space)
	{
		// Return position in local space (no evaluation needed)
		if (space == Space::LOCAL) {
			return transform.position;
		}
		// Extract position from world transform matrix
		else {
			return Transformation::toBackendPosition(glm::vec3(transform.model[3]));
		}
	}

	// Get rotation in specified space
	glm::quat getRotation(TransformComponent& transform, Space space)
	{
		// Return rotation in local space (no evaluation needed)
		if (space == Space::LOCAL) {
			return transform.rotation;
		}
		// Extract rotation from world transform matrix
		else {
			// Extract rotation from the model matrix by removing scale
			glm::mat3 rotationMatrix(transform.model);

			// Normalize each column to remove scale influence
			glm::vec3 col0 = glm::normalize(glm::vec3(rotationMatrix[0]));
			glm::vec3 col1 = glm::normalize(glm::vec3(rotationMatrix[1]));
			glm::vec3 col2 = glm::normalize(glm::vec3(rotationMatrix[2]));

			rotationMatrix[0] = col0;
			rotationMatrix[1] = col1;
			rotationMatrix[2] = col2;

			// Convert the orthonormal 3x3 matrix to a quaternion
			return Transformation::toBackendRotation(glm::quat_cast(rotationMatrix));
		}
	}

	// Get scale in specified space
	glm::vec3 getScale(TransformComponent& transform, Space space)
	{
		// Return scale in local space (no evaluation needed)
		if (space == Space::LOCAL) {
			return transform.scale;
		}
		// Extract scale from world transform matrix
		else {
			// Extract scale by getting the length of each basis vector
			return glm::vec3(
				glm::length(glm::vec3(transform.model[0])),
				glm::length(glm::vec3(transform.model[1])),
				glm::length(glm::vec3(transform.model[2]))
			);
		}
	}

	void translate(TransformComponent& transform, const glm::vec3& position, Space space)
	{
		// Set position in local space
		if (space == Space::LOCAL) {
			transform.position += position;
		}
		// Set position in world space
		else {

		}
		transform.modified = true;
	}

	void rotate(TransformComponent& transform, const glm::quat& rotation, Space space)
	{
		// Set rotation in local space
		if (space == Space::LOCAL) {
			transform.rotation = rotation * transform.rotation;
		}
		// Set rotation in world space
		else {

		}
		transform.modified = true;
	}

	void scale(TransformComponent& transform, const glm::vec3& scale, Space space)
	{
		// Set scale in local space
		if (space == Space::LOCAL) {
			transform.scale *= scale;
		}
		// Set scale in world space
		else {

		}
		transform.modified = true;
	}

	glm::vec3 forward(const TransformComponent& transform)
	{
		glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 forward_vector = glm::rotate(transform.rotation, forward_local);
		return forward_vector;
	}

	glm::vec3 backward(const TransformComponent& transform)
	{
		glm::vec3 backward_local = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 backward_vector = glm::rotate(transform.rotation, backward_local);
		return backward_vector;
	}

	glm::vec3 right(const TransformComponent& transform)
	{
		glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 right_vector = glm::rotate(transform.rotation, right_local);
		return right_vector;
	}

	glm::vec3 left(const TransformComponent& transform)
	{
		glm::vec3 left_local = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 left_vector = glm::rotate(transform.rotation, left_local);
		return left_vector;
	}

	glm::vec3 up(const TransformComponent& transform)
	{
		glm::vec3 up_local = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 up_vector = glm::rotate(transform.rotation, up_local);
		return up_vector;
	}

	glm::vec3 down(const TransformComponent& transform)
	{
		glm::vec3 down_local = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 down_vector = glm::rotate(transform.rotation, down_local);
		return down_vector;
	}

	glm::quat rotate(const TransformComponent& transform, float degrees, glm::vec3 axis) {
		return glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::normalize(axis)));
	}

	glm::quat rotateX(const TransformComponent& transform, float degrees) {
		return glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	glm::quat rotateY(const TransformComponent& transform, float degrees) {
		return glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	glm::quat rotateZ(const TransformComponent& transform, float degrees) {
		return glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(0.0f, 0.0f, 1.0f)));
	}

	glm::quat lookAt(const TransformComponent& transform, const TransformComponent& target)
	{
		// Get direction vector from current position to target position
		glm::vec3 direction = glm::normalize(target.position - transform.position);

		// Get local forward vector
		glm::vec3 forwardLocal = glm::vec3(0.0f, 0.0f, 1.0f);

		// Get rotation that aligns forward direction with direction
		glm::quat rotation = glm::rotation(forwardLocal, direction);

		return rotation;
	}

	glm::quat lookFromAt(const glm::vec3 from, const TransformComponent& target)
	{
		glm::vec3 direction = glm::normalize(target.position - from);
		glm::vec3 forwardLocal = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::quat rotation = glm::rotation(forwardLocal, direction);
		return rotation;
	}

	glm::quat fromEuler(float x, float y, float z)
	{
		return glm::quat(glm::vec3(glm::radians(x), glm::radians(y), glm::radians(z)));
	}

}
