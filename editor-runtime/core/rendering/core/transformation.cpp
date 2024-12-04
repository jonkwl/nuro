#include "transformation.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../core/entity/entity.h"
#include "../core/camera/camera.h"

namespace Transformation {

	glm::vec3 prepareWorldPosition(const glm::vec3 position)
	{
		// Swap between left-handed coordinates and right-handed coordinates
		return glm::vec3(position.x, position.y, -position.z);
	}

	glm::vec3 prepareWorldEulerAngles(const glm::vec3 rotation)
	{
		// Swap between left-handed euler angles and right-handed euler angles
		return glm::vec3(-rotation.x, -rotation.y, rotation.z);
	}

	glm::mat4 modelMatrix(Transform& transform)
	{
		glm::mat4 model(1.0f);

		// object position
		glm::vec3 worldPosition = prepareWorldPosition(transform.position);
		model = glm::translate(model, worldPosition);

		// object rotation
		glm::mat4 rotationMatrix = glm::mat4_cast(transform.rotation);
		model = model * rotationMatrix;

		// object scale
		model = glm::scale(model, transform.scale);

		return model;
	}

	glm::mat4 viewMatrix(Camera& camera)
	{
		// Extract camera position and orientation
		const glm::vec3& position = prepareWorldPosition(camera.transform.position);
		const glm::quat& rotation = camera.transform.rotation;

		// Create a rotation matrix from the quaternion
		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

		// Create a translation matrix for the camera position
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -position);

		// Combine rotation and translation to form the view matrix
		// Since the camera is moving in the opposite direction in the view space,
		// apply translation AFTER the rotation
		glm::mat4 viewMatrix = rotationMatrix * translationMatrix;

		return viewMatrix;
	}

	glm::mat4 projectionMatrix(Camera& camera, const Viewport& viewport)
	{
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), viewport.width / viewport.height, camera.getNear(), camera.getFar());
		return projection;
	}

	glm::mat4 lightViewMatrix(const glm::vec3 lightPosition, const glm::vec3 lightDirection)
	{
		glm::vec3 position = prepareWorldPosition(lightPosition);
		glm::vec3 target = position + glm::normalize(prepareWorldPosition(lightDirection));
		glm::mat4 view = glm::lookAt(
			position,
			target,
			glm::vec3(0.0f, 1.0f, 0.0f));
		return view;
	}

	glm::mat4 lightProjectionMatrix(const float boundsWidth, const float boundsHeight, const float near, const float far)
	{
		glm::mat4 projection = glm::ortho(-boundsWidth * 0.5f, boundsWidth * 0.5f, -boundsHeight * 0.5f, boundsHeight * 0.5f, near, far);
		return projection;
	}

}