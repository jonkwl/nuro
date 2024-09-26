#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class VectorHelper
{
public:
	static glm::vec3 forward(const glm::vec3& rotation);
	static glm::vec3 right(const glm::vec3& rotation);
};

