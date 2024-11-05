#include "inspector_mode.h"

float InspectorMode::movementSpeed = 12.0f;
float InspectorMode::sensitivity = 3.0f;

void InspectorMode::refreshInspector() {

	Camera* camera = Runtime::getInspectorCamera();

	glm::vec3 cam_forward = VectorHelper::forward(camera->rotation);
	glm::vec3 cam_right = VectorHelper::right(camera->rotation);

	glm::vec3 movement_direction = cam_forward * Input::keyAxisSmooth.x + cam_right * Input::keyAxisSmooth.y;
	camera->position += movement_direction * movementSpeed * Runtime::deltaTime;

	if (!Runtime::showEngineUI) {

		glm::vec3 rotate_direction = glm::vec3(-Input::mouseAxis.y, Input::mouseAxis.x, 0.0f);
		glm::vec3 new_rotation = camera->rotation + (rotate_direction * sensitivity * Runtime::deltaTime);

		new_rotation = glm::vec3(glm::clamp(new_rotation.x, -90.0f, 90.0f), new_rotation.y, new_rotation.z);
		camera->rotation = new_rotation;

	}
}