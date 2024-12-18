#pragma once

#include <glm.hpp>

#include "../core/transform/transform.h"
#include "../core/camera/frustum.h"
#include "../core/viewport/viewport.h"

class Camera
{
public:
	explicit Camera(float fov = 70.0f, float near = 0.3f, float far = 1000.0f);

	float getFov();
	float getNear();
	float getFar();

	void updateFrustum(Viewport& viewport);
	const Frustum& getFrustum();

private:
	Frustum frustum;

	float fov;
	float near;
	float far;

	glm::vec4 calculatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
};