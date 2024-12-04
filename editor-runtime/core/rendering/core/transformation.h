#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/transform/transform.h"
#include "../core/camera/camera.h"

class Entity;

namespace Transformation
{
	glm::vec3 prepareWorldPosition(const glm::vec3 position); // Swap between left-handed coordinates and right-handed coordinates
	glm::vec3 prepareWorldEulerAngles(const glm::vec3 rotation); // Swap between left-handed euler angles and right-handed euler angles
	
	glm::mat4 modelMatrix(Transform& transform); // Model matrix
	glm::mat4 viewMatrix(Camera& camera); // View matrix
	glm::mat4 projectionMatrix(Camera& camera, const Viewport& viewport); // Projection matrix
	
	glm::mat4 lightViewMatrix(const glm::vec3 lightPosition, const glm::vec3 lightDirection);
	glm::mat4 lightProjectionMatrix(const float boundsWidth, const float boundsHeight, const float near, const float far);
};