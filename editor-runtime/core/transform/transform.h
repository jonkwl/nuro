#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>

class Entity;

struct Transform
{
	Transform();

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	glm::vec3 getEulerAngles();
	void setEulerAngles(glm::vec3 eulerAngles);

	glm::vec3 forward();
	glm::vec3 backward();
	glm::vec3 right();
	glm::vec3 left();
	glm::vec3 up();
	glm::vec3 down();
};