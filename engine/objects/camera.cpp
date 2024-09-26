#include "camera.h"

Camera::Camera() {
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);

	fov = 70.0f;
	nearClipping = 0.1f;
	farClipping = 100.0f;
}