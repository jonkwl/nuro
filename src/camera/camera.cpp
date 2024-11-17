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
    // Left
    frustum.leftPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0]
    );

    // Right
    frustum.rightPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0]
    );

    // Bottom
    frustum.bottomPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1]
    );

    // Top
    frustum.topPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1]
    );

    // Near
    frustum.nearPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2]
    );

    // Far
    frustum.farPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2]
    );

    // Normalize all planes
    frustum.leftPlane /= glm::length(glm::vec3(frustum.leftPlane));
    frustum.rightPlane /= glm::length(glm::vec3(frustum.rightPlane));
    frustum.bottomPlane /= glm::length(glm::vec3(frustum.bottomPlane));
    frustum.topPlane /= glm::length(glm::vec3(frustum.topPlane));
    frustum.nearPlane /= glm::length(glm::vec3(frustum.nearPlane));
    frustum.farPlane /= glm::length(glm::vec3(frustum.farPlane));
}

Frustum Camera::getFrustum() const
{
	return frustum;
}