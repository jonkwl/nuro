#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "../src/core/rendering/transformation/transformation.h"

namespace Transform {

	void evaluate(TransformComponent& transform, const glm::mat4 viewProjection)
	{
		// Compute model matrix
		transform.model = Transformation::model(transform.position, transform.rotation, transform.scale);
		
		if (transform.parent) {
			transform.model = Transformation::model(transform.parent->position, transform.parent->rotation, transform.parent->scale) * transform.model;
		}

		// Compute model-view-projection matrix
		transform.mvp = viewProjection * transform.model;

		// Compute normal matrix
		transform.normal = Transformation::normal(transform.model);
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
