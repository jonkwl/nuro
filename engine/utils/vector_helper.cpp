#include "vector_helper.h"

glm::vec3 VectorHelper::forward(const glm::vec3& rotation) {
	// Convert Euler angles (in degrees) to radians
	glm::vec3 rotation_rad = glm::radians(rotation);

	// Create a quaternion from the Euler angles
	glm::quat quaternion = glm::quat(rotation_rad);

	// Define the forward vector in local space (typically negative Z-axis)
	glm::vec3 forward_local = glm::vec3(0.0f, 0.0f, 1.0f); // Points forward in local space

	// Rotate the local forward vector by the quaternion
	glm::vec3 forward_vector = quaternion * forward_local;

	// Return the forward vector
	return forward_vector;
}

glm::vec3 VectorHelper::right(const glm::vec3& rotation) {
	glm::vec3 rotation_rad = glm::radians(rotation);
	glm::quat quaternion = glm::quat(rotation_rad);
	glm::vec3 right_local = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right_vector = quaternion * right_local;
	return right_vector;
}