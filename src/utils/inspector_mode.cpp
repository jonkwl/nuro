#include "inspector_mode.h"

#include "../src/runtime/runtime.h"
#include "../src/camera/camera.h"
#include "../src/input/input.h"

namespace InspectorMode {

	float _movementSpeed = 12.0f;
	float _sensitivity = 0.08f;

	void _updateMovementMode(Camera* camera, float speed, float sensitivity) {
		glm::vec3 cam_forward = camera->transform.forward();
		glm::vec3 cam_right = camera->transform.right();

		glm::vec3 movement_direction = cam_forward * Input::keyAxisSmooth.x + cam_right * Input::keyAxisSmooth.y;
		camera->transform.position += movement_direction * speed * Runtime::deltaTime;

		if (!Runtime::showEngineUI) {

			glm::vec3 rotate_direction = glm::vec3(-Input::mouseAxis.y, Input::mouseAxis.x, 0.0f);
			glm::vec3 new_rotation = camera->transform.rotation + (rotate_direction * sensitivity);

			new_rotation = glm::vec3(glm::clamp(new_rotation.x, -90.0f, 90.0f), new_rotation.y, new_rotation.z);
			camera->transform.rotation = new_rotation;

		}
	}

	// mode just for testing purposes
	float _currentAngle = 0.0f;
	void _updateRotationMode(Camera* camera, glm::vec3 centerPoint, float radius, float speed) {
		_currentAngle += speed * Runtime::deltaTime;

		float angleRadians = glm::radians(_currentAngle);

		float x = centerPoint.x + radius * glm::cos(angleRadians);
		float z = centerPoint.z + radius * glm::sin(angleRadians);
		float y = camera->transform.position.y;

		camera->transform.position = glm::vec3(x, y, z);
		camera->transform.lookAt(centerPoint);
	}

	void refreshInspector() {

		Camera* camera = Runtime::getInspectorCamera();
		_updateMovementMode(camera, _movementSpeed, _sensitivity);
	}

}