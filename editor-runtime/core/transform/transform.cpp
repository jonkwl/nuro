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

glm::vec3 Transform::getEulerAngles()
{
	// Get euler angles from rotation quaternion in radians
	glm::vec3 eulerAngles = glm::eulerAngles(rotation);

	// Convert euler angles to degrees
	eulerAngles = glm::degrees(eulerAngles);

	// Convert euler angles to left-handed coordinate system
	eulerAngles = glm::vec3(-eulerAngles.x, eulerAngles.y, eulerAngles.z);

	// Return transform rotation as euler angles
	return eulerAngles;
}

void Transform::setEulerAngles(glm::vec3 eulerAngles)
{
	// Convert euler angles to use right-handed coordinate system
	eulerAngles = Transformation::prepareWorldEulerAngles(eulerAngles);

	// Convert euler angles given in degrees to radians
	eulerAngles = glm::radians(eulerAngles);

	// Rotate transform
	rotation = glm::quat(glm::mat4(1.0f));
	rotation = glm::rotate(rotation, eulerAngles.x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, eulerAngles.y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, eulerAngles.z, glm::vec3(0, 0, 1));
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
