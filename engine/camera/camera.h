#pragma once

#include <glm.hpp>

#include "../engine/transform/transform.h"

class Camera
{
public:
	Camera();

	Transform transform;

	float fov;
	float nearClipping;
	float farClipping;

	void lookAt(glm::vec3 point);
};