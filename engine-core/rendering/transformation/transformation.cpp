#include "transformation.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Transformation {

	glm::vec3 toBackendPosition(const glm::vec3& position)
	{
		// Use to convert left handed coordinates to right handed coordinates backend uses
		return glm::vec3(position.x, position.y, -position.z);
	}

	glm::quat toBackendRotation(const glm::quat& rotation)
	{
		// Use to convert left handed rotation to right handed rotation backend uses
		return glm::quat(rotation.w, -rotation.x, -rotation.y, rotation.z);
	}

	glm::mat4 model(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
	{
		glm::mat4 model(1.0f);

		// Convert left handed transform position to right handed position
		glm::vec3 backendPosition = toBackendPosition(position);

		// Transpose model matrix
		model = glm::translate(model, backendPosition);

		// Convert left handed rotation to right handed rotation
		glm::quat backendRotation = toBackendRotation(rotation);

		// Normalize rotation
		backendRotation = glm::normalize(backendRotation);

		// Get rotation matrix and rotate model matrix
		glm::mat4 rotationMatrix = glm::mat4_cast(backendRotation);
		model = model * rotationMatrix;

		// Scale model matrix
		model = glm::scale(model, scale);

		return model;
	}

	glm::mat4 view(const glm::vec3& cameraPosition, const glm::quat& cameraRotation)
	{
		glm::vec3 position = toBackendPosition(cameraPosition);
		glm::quat rotation = toBackendRotation(cameraRotation);

		// Normalize rotation
		rotation = glm::normalize(rotation);

		// Convert quaternion to matrix
		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

		// Calculate the forward direction
		glm::vec3 forward = rotationMatrix * glm::vec4(0, 0, -1, 0); // Forward vector
		glm::vec3 up = rotationMatrix * glm::vec4(0, 1, 0, 0);		 // Up vector

		// Calculate the target position
		glm::vec3 target = position + forward;

		// Create and return view matrix
		return glm::lookAt(position, target, up);
	}

	glm::mat4 projection(float fov, float aspect, float near, float far)
	{
		// Create and return projection matrix
		return glm::perspective(glm::radians(fov), aspect, near, far);
	}

	glm::mat4 normal(const glm::mat4& model)
	{
		// Calculate normal matrix
		return glm::transpose(glm::inverse(model));
	}

}