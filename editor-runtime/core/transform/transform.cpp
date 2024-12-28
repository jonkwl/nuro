#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "../core/rendering/core/transformation.h"

namespace Transform {

	void evaluate(TransformComponent& transform, const glm::mat4 viewProjection)
	{
		// Apply parents transform components to transform
		if (transform.parent) {
			transform.position += transform.parent->position;
			transform.rotation = transform.parent->rotation * transform.rotation;
			transform.scale *= transform.parent->scale;
		}

		// Calculate transformation matrices
		transform.model = Transformation::model(transform);
		transform.mvp = viewProjection * transform.model;
		transform.normal = Transformation::normal(transform.model);
	}

	glm::vec3 forward(TransformComponent& transform)
	{
		glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 forward_vector = glm::rotate(transform.rotation, forward_local);
		return forward_vector;
	}

	glm::vec3 backward(TransformComponent& transform)
	{
		glm::vec3 backward_local = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 backward_vector = glm::rotate(transform.rotation, backward_local);
		return backward_vector;
	}

	glm::vec3 right(TransformComponent& transform)
	{
		glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 right_vector = glm::rotate(transform.rotation, right_local);
		return right_vector;
	}

	glm::vec3 left(TransformComponent& transform)
	{
		glm::vec3 left_local = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 left_vector = glm::rotate(transform.rotation, left_local);
		return left_vector;
	}

	glm::vec3 up(TransformComponent& transform)
	{
		glm::vec3 up_local = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 up_vector = glm::rotate(transform.rotation, up_local);
		return up_vector;
	}

	glm::vec3 down(TransformComponent& transform)
	{
		glm::vec3 down_local = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 down_vector = glm::rotate(transform.rotation, down_local);
		return down_vector;
	}

	void rotate(TransformComponent& transform, float degrees, glm::vec3 axis) {
		transform.rotation = glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::normalize(axis)));
	}

	void rotateX(TransformComponent& transform, float degrees) {
		transform.rotation = glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	void rotateY(TransformComponent& transform, float degrees) {
		transform.rotation = glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void rotateZ(TransformComponent& transform, float degrees) {
		transform.rotation = glm::normalize(transform.rotation * glm::angleAxis(glm::radians(degrees), glm::vec3(0.0f, 0.0f, 1.0f)));
	}

	glm::quat fromEuler(float x, float y, float z)
	{
		return glm::quat(glm::vec3(glm::radians(x), glm::radians(y), glm::radians(z)));
	}

}
