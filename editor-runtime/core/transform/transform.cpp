#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "../core/rendering/core/transformation.h"

namespace Transform {

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

}
