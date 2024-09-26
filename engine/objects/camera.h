#pragma once

#include <glm.hpp>

class Camera
{
	Camera();

	glm::vec3 position;
	glm::vec3 rotation;

	float fov;
	float nearClipping;
	float farClipping;
};