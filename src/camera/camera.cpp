#include "camera.h"

#include "../src/window/window.h"

Camera::Camera() {
    transform = Transform();

	fov = 70.0f;
	near = 0.3f;
	far = 1000.0f;

	frustum = Frustum();
}

void Camera::updateFrustum(glm::mat4 viewProjectionMatrix)
{
	// Calculate camera frustums planes by view projection matrix
	frustum.leftPlane = glm::vec4(viewProjectionMatrix[3] + viewProjectionMatrix[0]);
	frustum.rightPlane = glm::vec4(viewProjectionMatrix[3] - viewProjectionMatrix[0]);
	frustum.bottomPlane = glm::vec4(viewProjectionMatrix[3] + viewProjectionMatrix[1]);
	frustum.topPlane = glm::vec4(viewProjectionMatrix[3] - viewProjectionMatrix[1]);
	frustum.nearPlane = glm::vec4(viewProjectionMatrix[3] + viewProjectionMatrix[2]);
	frustum.farPlane = glm::vec4(viewProjectionMatrix[3] - viewProjectionMatrix[2]);

	// Normalize each frustum plane
	frustum.leftPlane = glm::normalize(frustum.leftPlane);
	frustum.rightPlane = glm::normalize(frustum.rightPlane);
	frustum.bottomPlane = glm::normalize(frustum.bottomPlane);
	frustum.topPlane = glm::normalize(frustum.topPlane);
	frustum.nearPlane = glm::normalize(frustum.nearPlane);
	frustum.farPlane = glm::normalize(frustum.farPlane);
}

Frustum Camera::getFrustum() const
{
	return frustum;
}