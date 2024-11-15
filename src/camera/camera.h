#pragma once

#include <glm.hpp>

#include "../src/transform/transform.h"

class Camera
{
public:
	Camera();

	Transform transform;

	float fov;
	float nearClipping;
	float farClipping;
};