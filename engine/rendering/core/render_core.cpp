#include "render_core.h"

glm::mat4 RenderCore::model_matrix(Entity* entity) {
	glm::mat4 model(1.0f);

	// object position
	model = glm::translate(model, glm::vec3(entity->position.x, entity->position.y, -entity->position.z));

	// object rotation
	model = glm::rotate(model, glm::radians(-entity->rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(-entity->rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(entity->rotation.z), glm::vec3(0, 0, 1));

	// object scale
	model = glm::scale(model, entity->scale);

	return model;
}

glm::mat4 RenderCore::view_matrix(Camera* camera) {
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

glm::mat4 RenderCore::projection_matrix(Camera* camera, int width, int height) {
	return glm::perspective(glm::radians(camera->fov), (float)width / (float)height, camera->nearClipping, camera->farClipping);
}

glm::mat4 RenderCore::mvp(Entity* entity, Camera* camera, int width, int height) {
	glm::mat4 model = model_matrix(entity);
	glm::mat4 view = view_matrix(camera);
	glm::mat4 projection = projection_matrix(camera, width, height);
	return projection * view * model;
}