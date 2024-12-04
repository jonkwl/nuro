#pragma once

#include <glm.hpp>

#include "../core/transform/transform.h"
#include "../core/camera/frustum.h"

class Camera
{
public:
	explicit Camera(float fov = 70.0f, float near = 0.3f, float far = 1000.0f);

	Transform transform;

	glm::vec3 eulerAngles;

	float getFov();
	float getNear();
	float getFar();

	void updateFrustum(glm::mat4 viewProjectionMatrix);
	Frustum& getFrustum();

private:
	Frustum frustum;

	float fov;
	float near;
	float far;
};