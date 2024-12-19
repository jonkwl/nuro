#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/ecs/components.h"

class OldEntity;

namespace Transformation
{
	glm::vec3 toBackendPosition(glm::vec3 position); // Use to convert left handed coordinates to right handed coordinates backend uses
	glm::quat toBackendRotation(glm::quat rotation); // Use to convert left handed rotation to right handed rotation backend uses
	
	glm::mat4 model(TransformComponent& transform); // Model matrix
	glm::mat4 view(glm::vec3 cameraPosition, glm::quat cameraRotation); // View matrix
	glm::mat4 projection(float fov, float near, float far, const Viewport& viewport); // Projection matrix
	
	glm::mat4 lightView(glm::vec3 lightPosition, glm::vec3 lightDirection);
	glm::mat4 lightProjection(float boundsWidth, float boundsHeight, float near, float far);
};