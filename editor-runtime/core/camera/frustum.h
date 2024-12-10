#pragma once

#include <glm.hpp>

struct Frustum
{
	glm::vec4 leftPlane;
	glm::vec4 rightPlane;
	glm::vec4 topPlane;
	glm::vec4 bottomPlane;
	glm::vec4 nearPlane;
	glm::vec4 farPlane;
};