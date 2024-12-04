#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "../core/entity/entity.h"
#include "../core/rendering/core/transformation.h"

Transform::Transform() : position(glm::vec3(0.0f)),
rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
scale(glm::vec3(1.0f))
{
}

void Transform::setEulerAngles(glm::vec3 eulerAngles)
{
	// Convert Euler angles from degrees to radians
	eulerAngles = glm::radians(eulerAngles);

	// Set rotation quaternion
	rotation = glm::quat(eulerAngles);
}

glm::vec3 Transform::forward()
{
	glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 forward_vector = glm::rotate(rotation, forward_local);
	return forward_vector;
}

glm::vec3 Transform::backward()
{
	glm::vec3 backward_local = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 backward_vector = glm::rotate(rotation, backward_local);
	return backward_vector;
}

glm::vec3 Transform::right()
{
	glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right_vector = glm::rotate(rotation, right_local);
	return right_vector;
}

glm::vec3 Transform::left()
{
	glm::vec3 left_local = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 left_vector = glm::rotate(rotation, left_local);
	return left_vector;
}

glm::vec3 Transform::up()
{
	glm::vec3 up_local = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 up_vector = glm::rotate(rotation, up_local);
	return up_vector;
}

glm::vec3 Transform::down()
{
	glm::vec3 down_local = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 down_vector = glm::rotate(rotation, down_local);
	return down_vector;
}
