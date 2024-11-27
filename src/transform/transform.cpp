#include "transform.h"

#include <gtc/quaternion.hpp>
#include "../src/entity/entity.h"

Transform::Transform()
{
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
}

glm::vec3 Transform::forward()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 forward_vector = quaternion * forward_local;
	return forward_vector;
}

glm::vec3 Transform::backward()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 backward_local = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 backward_vector = quaternion * backward_local;
	return backward_vector;
}

glm::vec3 Transform::right()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right_vector = quaternion * right_local;
	return right_vector;
}

glm::vec3 Transform::left()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 left_local = glm::vec3(-1.0f, 0.0f, 0.0f);
	glm::vec3 left_vector = quaternion * left_local;
	return left_vector;
}

glm::vec3 Transform::up()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 up_local = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 up_vector = quaternion * up_local;
	return up_vector;
}

glm::vec3 Transform::down()
{
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 down_local = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 down_vector = quaternion * down_local;
	return down_vector;
}

void Transform::lookAt(glm::vec3 point)
{
	glm::vec3 direction = point - position;
	direction = glm::normalize(direction);

	float pitch = glm::degrees(glm::atan(-direction.y, glm::length(glm::vec2(direction.x, direction.z))));
	float yaw = glm::degrees(glm::atan(direction.x, direction.z));

	rotation = glm::vec3(pitch, yaw, 0.0f);
}

void Transform::lookAt(Entity *entity)
{
	lookAt(entity->transform.position);
}
