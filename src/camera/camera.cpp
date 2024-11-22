#include "camera.h"

#include "../src/window/window.h"

Camera::Camera() {
    transform = Transform();

    frustum = Frustum();

	fov = 70.0f;
	near = 0.3f;
	far = 1000.0f;
}

float Camera::getFov()
{
    // Return y fov
    return fov;
}

float Camera::getNear()
{
    // Return near plane
    return near;
}

float Camera::getFar()
{
    // Return far plane
    return far;
}

void Camera::updateFrustum(glm::mat4 viewProjectionMatrix)
{
    // Extract columns of the matrix
    glm::vec4 col0 = glm::column(viewProjectionMatrix, 0);
    glm::vec4 col1 = glm::column(viewProjectionMatrix, 1);
    glm::vec4 col2 = glm::column(viewProjectionMatrix, 2);
    glm::vec4 col3 = glm::column(viewProjectionMatrix, 3);

    // Compute planes
    frustum.planes[0] = col3 + col0; // Left
    frustum.planes[1] = col3 - col0; // Right
    frustum.planes[2] = col3 + col1; // Bottom
    frustum.planes[3] = col3 - col1; // Top
    frustum.planes[4] = col3 + col2; // Near
    frustum.planes[5] = col3 - col2; // Far

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(frustum.planes[i]));
        frustum.planes[i] /= length;
    }
}