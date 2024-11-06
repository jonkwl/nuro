#pragma once

#include <glm.hpp>

class Camera
{
public:
	Camera();

	glm::vec3 position;
	glm::vec3 rotation;

	float fov;
	float nearClipping;
	float farClipping;

	void lookAt(glm::vec3 point);
};