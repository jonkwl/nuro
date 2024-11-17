#include "frustum.h"

void Frustum::update(glm::mat4 viewProjectionMatrix)
{
    // Left
    leftPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0]
    );

    // Right
    rightPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0]
    );

    // Bottom
    bottomPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1]
    );

    // Top
    topPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1]
    );

    // Near
    nearPlane = glm::vec4(
        viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2],
        viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2]
    );

    // Far
    farPlane = glm::vec4(
        viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
        viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
        viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2],
        viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2]
    );

    // Normalize all planes
    leftPlane /= glm::length(glm::vec3(leftPlane));
    rightPlane /= glm::length(glm::vec3(rightPlane));
    bottomPlane /= glm::length(glm::vec3(bottomPlane));
    topPlane /= glm::length(glm::vec3(topPlane));
    nearPlane /= glm::length(glm::vec3(nearPlane));
    farPlane /= glm::length(glm::vec3(farPlane));
}