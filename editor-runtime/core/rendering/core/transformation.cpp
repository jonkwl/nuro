#include "transformation.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Transformation {

	glm::vec3 toBackendPosition(glm::vec3 position)
	{
		// Use to convert left handed coordinates to right handed coordinates backend uses
		return glm::vec3(position.x, position.y, -position.z);
	}

	glm::quat toBackendRotation(glm::quat rotation)
	{
		// Use to convert left handed rotation to right handed rotation backend uses
		rotation.x = -rotation.x;
		rotation.y = -rotation.y;
		rotation.z = rotation.z;
		return rotation;
	}

	glm::mat4 model(TransformComponent& transform)
	{
		glm::mat4 model(1.0f);

		// Convert left handed transform position to right handed position
		glm::vec3 worldPosition = toBackendPosition(transform.position);

		// Transpose model matrix
		model = glm::translate(model, worldPosition);

		// Convert left handed rotation to right handed rotation
		glm::quat rotation = toBackendRotation(transform.rotation);

		// Normalize rotation
		rotation = glm::normalize(rotation);

		// Get rotation matrix and rotate model matrix
		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
		model = model * rotationMatrix;

		// Scale model matrix
		model = glm::scale(model, transform.scale);

		return model;
	}

	glm::mat4 view(glm::vec3 cameraPosition, glm::quat cameraRotation)
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

		// Create the view matrix
		glm::mat4 viewMatrix = glm::lookAt(position, target, up);

		return viewMatrix;
	}

	glm::mat4 projection(float fov, float near, float far, const Viewport& viewport)
	{
		glm::mat4 projection = glm::perspective(glm::radians(fov), viewport.width / viewport.height, near, far);
		return projection;
	}

	glm::mat4 lightView(glm::vec3 lightPosition, glm::vec3 lightDirection)
	{
		glm::vec3 position = toBackendPosition(lightPosition);
		glm::vec3 target = position + glm::normalize(toBackendPosition(lightDirection));
		glm::mat4 view = glm::lookAt(
			position,
			target,
			glm::vec3(0.0f, 1.0f, 0.0f));
		return view;
	}

	glm::mat4 lightProjection(float boundsWidth, float boundsHeight, float near, float far)
	{
		glm::mat4 projection = glm::ortho(-boundsWidth * 0.5f, boundsWidth * 0.5f, -boundsHeight * 0.5f, boundsHeight * 0.5f, near, far);
		return projection;
	}

}