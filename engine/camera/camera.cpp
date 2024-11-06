#include "camera.h"

Camera::Camera() {
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);

	fov = 70.0f;
	nearClipping = 0.3f;
	farClipping = 1000.0f;
}

void Camera::lookAt(glm::vec3 point)
{
    glm::vec3 direction = point - position;
    direction = glm::normalize(direction);

    float pitch = glm::degrees(glm::atan(-direction.y, glm::length(glm::vec2(direction.x, direction.z))));
    float yaw = glm::degrees(glm::atan(direction.x, direction.z));

    rotation = glm::vec3(pitch, yaw, 0.0f);
}