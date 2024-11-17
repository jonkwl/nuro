#pragma once

#include <glm.hpp>

#include "../src/camera/frustum.h"
#include "../src/transform/transform.h"

class Camera
{
public:
	Camera();

	Transform transform;

	Frustum frustum;

	float fov;
	float near;
	float far;
};