#pragma once

#include <glm.hpp>

class Entity;

class Transform
{
public:
	Transform();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 forward();
	glm::vec3 backward();
	glm::vec3 right();
	glm::vec3 left();
	glm::vec3 up();
	glm::vec3 down();

	void lookAt(glm::vec3 point);
	void lookAt(Entity* entity);
};

