#pragma once

#include <glm.hpp>

#include "../src/transform/transform.h"

struct Frustum
{
	glm::vec4 planes[6];
};

class Camera
{
public:
	explicit Camera(float fov = 70.0f, float near = 0.3f, float far = 1000.0f);

	Frustum frustum;
	Transform transform;

	float getFov();
	float getNear();
	float getFar();

	void updateFrustum(glm::mat4 viewProjectionMatrix);

private:
	float fov;
	float near;
	float far;
};