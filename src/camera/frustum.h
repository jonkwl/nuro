#pragma once

#include <glm.hpp>

class Frustum
{
public:
	glm::vec4 leftPlane;
	glm::vec4 rightPlane;
	glm::vec4 bottomPlane;
	glm::vec4 topPlane;
	glm::vec4 nearPlane;
	glm::vec4 farPlane;

	void update(glm::mat4 viewProjectionMatrix);
};