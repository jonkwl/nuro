#include "transformation.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Transformation {

	glm::vec3 swap(const glm::vec3& position)
	{
		return glm::vec3(position.x, position.y, -position.z);
	}

	glm::quat swap(const glm::quat& rotation)
	{
		return glm::quat(rotation.w, -rotation.x, -rotation.y, rotation.z);
	}

	glm::mat4 model(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
	{
		glm::mat4 model(1.0f);

		model = glm::translate(model, swap(position));
		model *= glm::mat4_cast(glm::normalize(swap(rotation)));
		model = glm::scale(model, scale);

		return model;
	}

	glm::mat4 view(const glm::vec3& cameraPosition, const glm::quat& cameraRotation)
	{
		glm::vec3 position = swap(cameraPosition);
		glm::quat rotation = swap(cameraRotation);

		rotation = glm::normalize(rotation);

		glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);

		return glm::lookAt(position, position + forward, up);
	}

	glm::mat4 projection(float fov, float aspect, float near, float far)
	{
		return glm::perspective(glm::radians(fov), aspect, near, far);
	}

	glm::mat4 normal(const glm::mat4& model)
	{
		return glm::transpose(glm::inverse(glm::mat3(model)));
	}

}