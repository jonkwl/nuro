#include "transformation.h"

glm::vec3 Transformation::prepareWorldPosition(glm::vec3 position)
{
	return glm::vec3(position.x, position.y, -position.z);
}

glm::vec3 Transformation::prepareWorldRotation(glm::vec3 rotation)
{
	return glm::vec3(-rotation.x, -rotation.y, rotation.z);
}

glm::mat4 Transformation::modelMatrix(Entity* entity) {
	glm::mat4 model(1.0f);

	// object position
	glm::vec3 worldPosition = prepareWorldPosition(entity->position);
	model = glm::translate(model, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));

	// object rotation
	glm::vec3 worldRotation = prepareWorldRotation(entity->rotation);
	model = glm::rotate(model, glm::radians(worldRotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(worldRotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(worldRotation.z), glm::vec3(0, 0, 1));

	// object scale
	model = glm::scale(model, entity->scale);

	return model;
}
glm::mat4 Transformation::viewMatrix(Camera* camera) {
	glm::vec3 camera_position = camera->position;
	glm::vec3 camera_rotation = camera->rotation;

	camera_position = glm::vec3(camera_position.x, camera_position.y, -camera_position.z);
	camera_rotation = glm::vec3(-camera_rotation.x, -camera_rotation.y, camera_rotation.z);

	glm::vec3 radRotation = glm::radians(camera_rotation);

	// Create rotation quaternions
	glm::quat pitchQuat = glm::angleAxis(radRotation.x, glm::vec3(1, 0, 0)); // Pitch (X)
	glm::quat yawQuat = glm::angleAxis(radRotation.y, glm::vec3(0, 1, 0));   // Yaw (Y)
	glm::quat rollQuat = glm::angleAxis(radRotation.z, glm::vec3(0, 0, 1));  // Roll (Z)

	// Combine rotations
	glm::quat orientation = yawQuat * pitchQuat * rollQuat;

	// Convert quaternion to matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(orientation);

	// Calculate the forward direction
	glm::vec3 forward = rotationMatrix * glm::vec4(0, 0, -1, 0); // Forward vector
	glm::vec3 up = rotationMatrix * glm::vec4(0, 1, 0, 0);       // Up vector

	// Calculate the target position
	glm::vec3 target = camera_position + forward;

	// Create the view matrix
	glm::mat4 viewMatrix = glm::lookAt(camera_position, target, up);

	return viewMatrix;
}
glm::mat4 Transformation::projectionMatrix(Camera* camera, int width, int height) {
	return glm::perspective(glm::radians(camera->fov), (float)width / (float)height, camera->nearClipping, camera->farClipping);
}

glm::mat4 Transformation::lightViewMatrix(glm::vec3 lightPosition)
{
	glm::vec3 position = prepareWorldPosition(lightPosition);
	glm::mat4 view = glm::lookAt(
		position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	return view;
}

glm::mat4 Transformation::lightProjectionMatrix(Camera* camera)
{
	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, camera->nearClipping, camera->farClipping);
	return projection;
}
