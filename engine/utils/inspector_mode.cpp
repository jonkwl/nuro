#include "inspector_mode.h"

float InspectorMode::movementSpeed = 12.0f;
float InspectorMode::sensitivity = 0.08f;

glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
float radius = 6.0f;
float rotationSpeed = 20.0f;
float currentAngle = 0.0f;

void movementMode(Camera* camera, float speed, float sensitivity) {
	glm::vec3 cam_forward = VectorHelper::forward(camera->rotation);
	glm::vec3 cam_right = VectorHelper::right(camera->rotation);

	glm::vec3 movement_direction = cam_forward * Input::keyAxisSmooth.x + cam_right * Input::keyAxisSmooth.y;
	camera->position += movement_direction * speed * Runtime::deltaTime;

	if (!Runtime::showEngineUI) {

		glm::vec3 rotate_direction = glm::vec3(-Input::mouseAxis.y, Input::mouseAxis.x, 0.0f);
		glm::vec3 new_rotation = camera->rotation + (rotate_direction * sensitivity);

		new_rotation = glm::vec3(glm::clamp(new_rotation.x, -90.0f, 90.0f), new_rotation.y, new_rotation.z);
		camera->rotation = new_rotation;

	}
}

void rotationMode(Camera* camera) {
	currentAngle += rotationSpeed * Runtime::deltaTime;

	float angleRadians = glm::radians(currentAngle);

	float x = centerPoint.x + radius * glm::cos(angleRadians);
	float z = centerPoint.z + radius * glm::sin(angleRadians);
	float y = camera->position.y;

	camera->position = glm::vec3(x, y, z);
	camera->lookAt(centerPoint);
}

void InspectorMode::refreshInspector() {

	Camera* camera = Runtime::getInspectorCamera();
	movementMode(camera, movementSpeed, sensitivity);
}