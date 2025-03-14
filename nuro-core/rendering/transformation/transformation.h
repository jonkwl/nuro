#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Transformation
{
	// Converts a position between backend and user coordinates
	glm::vec3 swap(const glm::vec3& position);

	// Converts a rotation between backend and user coordinates
	glm::quat swap(const glm::quat& rotation);
	
	// Evaluates model matrix
	glm::mat4 model(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

	// Evaluates view matrix
	glm::mat4 view(const glm::vec3& cameraPosition, const glm::quat& cameraRotation);

	// Evaluates projection matrix
	glm::mat4 projection(float fov, float aspect, float near, float far);

	// Evaluates normal matrix (transposed inversed model matrix) of given model matrix
	glm::mat4 normal(const glm::mat4& model);
};