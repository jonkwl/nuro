#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/ecs/components.h"

namespace Transformation
{
	// Converts left handed coordinates to right handed coordinates backend uses
	glm::vec3 toBackendPosition(glm::vec3 position);

	// Converts left handed rotation to right handed rotation backend uses
	glm::quat toBackendRotation(glm::quat rotation);
	
	// Calculates model matrix by given transform component
	glm::mat4 model(const TransformComponent& transform);

	// Calculates view matrix by given spatial camera data
	glm::mat4 view(glm::vec3 cameraPosition, glm::quat cameraRotation);

	// Calculates projection matrix by given parameters
	glm::mat4 projection(float fov, float near, float far, const Viewport& viewport);

	// Calculates normal matrix (transposed inversed model matrix) by given model matrix
	glm::mat4 normal(glm::mat4 model);
	
	//
	// Tmp
	//

	glm::mat4 lightView(glm::vec3 lightPosition, glm::vec3 lightDirection);
	glm::mat4 lightProjection(float boundsWidth, float boundsHeight, float near, float far);
};