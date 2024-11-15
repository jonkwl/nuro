#pragma once

#include <glm.hpp>

#include "../src/transform/transform.h"

struct Frustum {
	glm::vec4 leftPlane;
	glm::vec4 rightPlane;
	glm::vec4 bottomPlane;
	glm::vec4 topPlane;
	glm::vec4 nearPlane;
	glm::vec4 farPlane;
};

class Camera
{
public:
	Camera();

	Transform transform;

	float fov;
	float near;
	float far;

	void updateFrustum(glm::mat4 viewProjectionMatrix);
	Frustum getFrustum() const;
private:
	Frustum frustum;
};