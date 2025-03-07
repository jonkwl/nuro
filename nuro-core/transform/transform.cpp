#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <rendering/transformation/transformation.h>

namespace Transform {

	void evaluate(TransformComponent& transform)
	{
		// Transform has parent
		if (transform.parent) {

			// Set transform to be modified if parent is modified
			if (transform.parent->modified) transform.modified = true;

			// Only evaluate transform if it has been modified
			if (!transform.modified) return;

			// Evaluate transforms model matrix relative to parent
			transform.model = transform.parent->model * Transformation::model(transform.position, transform.rotation, transform.scale);

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

	void createMvp(TransformComponent& transform, const glm::mat4& viewProjection)
	{
		transform.mvp = viewProjection * transform.model;
	}

	void setPosition(TransformComponent& transform, const glm::vec3& position, Space space)
	{
		if (position == transform.position) return;

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
