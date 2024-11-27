#pragma once

#include <glm.hpp>

#include "../src/transform/transform.h"

class Transform;

struct Frustum
{
	glm::vec4 planes[6];
};

class Camera
{
public:
	Camera();

	Transform transform;

	Frustum frustum;

	float getFov();
	float getNear();
	float getFar();

	void updateFrustum(glm::mat4 viewProjectionMatrix);

private:
	float fov;
	float near;
	float far;
};