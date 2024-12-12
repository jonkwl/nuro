#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/transform/transform.h"
#include "../core/camera/camera.h"

class OldEntity;

namespace Transformation
{
	glm::vec3 toBackendPosition(glm::vec3 position); // Use to convert left handed coordinates to right handed coordinates backend uses
	glm::quat toBackendRotation(glm::quat rotation); // Use to convert left handed rotation to right handed rotation backend uses
	
	glm::mat4 modelMatrix(Transform& transform); // Model matrix
	glm::mat4 viewMatrix(Camera& camera); // View matrix
	glm::mat4 projectionMatrix(Camera& camera, const Viewport& viewport); // Projection matrix
	
	glm::mat4 lightViewMatrix(glm::vec3 lightPosition, glm::vec3 lightDirection);
	glm::mat4 lightProjectionMatrix(float boundsWidth, float boundsHeight, float near, float far);
};