#include "camera.h"

Camera::Camera(float fov, float near, float far) : frustum(),
fov(fov),
near(near),
far(far)
{
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

void Camera::updateFrustum(Viewport& viewport)
{
    /*
    // Half field of view (vertical)
    float halfFov = glm::radians(fov * 0.5f);
    float aspectRatio = viewport.width / viewport.height;

    glm::vec3 position = transform.position;
    glm::vec3 forward = transform.forward();
    glm::vec3 right = transform.right();
    glm::vec3 up = transform.up();

    // Calculate near and far centers
    glm::vec3 nearCenter = position + forward * near;
    glm::vec3 farCenter = position + forward * far;

    // Calculate near and far heights/widths
    float nearHeight = 2.0f * tan(halfFov) * near;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = 2.0f * tan(halfFov) * far;
    float farWidth = farHeight * aspectRatio;

    // Calculate near and far plane corners
    glm::vec3 nearTopLeft = nearCenter + up * (nearHeight * 0.5f) - right * (nearWidth * 0.5f);
    glm::vec3 nearTopRight = nearCenter + up * (nearHeight * 0.5f) + right * (nearWidth * 0.5f);
    glm::vec3 nearBottomLeft = nearCenter - up * (nearHeight * 0.5f) - right * (nearWidth * 0.5f);
    glm::vec3 nearBottomRight = nearCenter - up * (nearHeight * 0.5f) + right * (nearWidth * 0.5f);

    glm::vec3 farTopLeft = farCenter + up * (farHeight * 0.5f) - right * (farWidth * 0.5f);
    glm::vec3 farTopRight = farCenter + up * (farHeight * 0.5f) + right * (farWidth * 0.5f);
    glm::vec3 farBottomLeft = farCenter - up * (farHeight * 0.5f) - right * (farWidth * 0.5f);
    glm::vec3 farBottomRight = farCenter - up * (farHeight * 0.5f) + right * (farWidth * 0.5f);

    // Calculate planes using the corners
    frustum.nearPlane = calculatePlane(nearTopLeft, nearTopRight, nearBottomLeft);
    frustum.farPlane = calculatePlane(farTopLeft, farTopRight, farBottomLeft);
    frustum.leftPlane = calculatePlane(nearTopLeft, farTopLeft, farBottomLeft);
    frustum.rightPlane = calculatePlane(nearTopRight, farTopRight, farBottomRight);
    frustum.topPlane = calculatePlane(nearTopLeft, farTopLeft, farTopRight);
    frustum.bottomPlane = calculatePlane(nearBottomLeft, farBottomLeft, farBottomRight);
    */
}

const Frustum& Camera::getFrustum()
{
	// Return frustum
	return frustum;
}

glm::vec4 Camera::calculatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    // Create two vectors from the three points
    glm::vec3 v1 = p2 - p1;
    glm::vec3 v2 = p3 - p1;

    // Calculate normal via cross product
    glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

    // Calculate distance (d) from origin to plane
    float d = -glm::dot(normal, p1);

    return glm::vec4(normal, d);
}
