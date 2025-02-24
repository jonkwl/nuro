#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Transformation
{
	// Converts left handed coordinates to right handed coordinates backend uses
	glm::vec3 toBackendPosition(const glm::vec3& position);

	// Converts left handed rotation to right handed rotation backend uses
	glm::quat toBackendRotation(const glm::quat& rotation);
	
	// Returns a model matrix by given transform component
	glm::mat4 model(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

	// Returns a view matrix by given spatial camera data
	glm::mat4 view(const glm::vec3& cameraPosition, const glm::quat& cameraRotation);

	// Returns a projection matrix by given parameters
	glm::mat4 projection(float fov, float aspect, float near, float far);

	// Returns a normal matrix (transposed inversed model matrix) by given model matrix
	glm::mat4 normal(const glm::mat4& model);
};